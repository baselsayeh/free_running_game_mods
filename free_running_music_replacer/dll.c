#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <DSound.h>
#include <errno.h>
#include "folder_file.h"

//the types of the origional functions
typedef HRESULT WINAPI (*DirectSoundCreate8_typedef)(LPCGUID lpcGuidDevice, LPDIRECTSOUND8 * ppDS8, LPUNKNOWN pUnkOuter);
typedef FILE *(*fopen_typedef)(const char *filename, const char *mode);
typedef size_t (*fread_typedef)(void *DstBuf, size_t ElementSize, size_t Count, FILE *File);
typedef int (*fseek_typedef)(FILE *File, int Offset, int Origin);
typedef void (*decrypt_music_typedef)(unsigned char *from, unsigned char *to, int len, char *i_v);

HINSTANCE ori_dsound_library;
DirectSoundCreate8_typedef _ori_DirectSoundCreate8;
fopen_typedef _ori_fopen;
fread_typedef _ori_fread;
fseek_typedef _ori_fseek;
decrypt_music_typedef ori_decrypt_music = (decrypt_music_typedef)0x51A0C0;
//

//
//variables
unsigned long file_length = 0;
unsigned char *file_in_mem = NULL;
unsigned long curr_file_offset = 0;


//needed for the player
FILE *EUR_PC_FP = NULL;

int last_Offset = 0;
int music_change = 0;
int curr_music = 0;

////int lock_music_controlls = 0;

int music_can_handle = 0;

int music_pause = 0;

//needed to get the origional addresses for the functions
HINSTANCE ms_ori_lib;
//

//another includes
#include "music_controls.h"
//
///////////////////////////////////////

//dll functions
HRESULT WINAPI DirectSoundCreate8(LPCGUID lpcGuidDevice, LPDIRECTSOUND8 * ppDS8, LPUNKNOWN pUnkOuter) {
	return _ori_DirectSoundCreate8(lpcGuidDevice, ppDS8, pUnkOuter);
}


//file handlers
FILE *fopen_wrapper(const char *filename, const char *mode) {
	FILE *fp_o;
	fp_o = _ori_fopen(filename, mode);

	if (strstr(filename, "EUR.PC") != NULL)
		if (EUR_PC_FP == NULL)
			EUR_PC_FP = fp_o;

	return fp_o;
}

int fseek_wrapper(FILE *File, int Offset, int Origin) {
	size_t ret;

	if (EUR_PC_FP == File)
		last_Offset = Offset;

	ret = _ori_fseek(File, Offset, Origin);

	return ret;
}

size_t fread_wrapper(void *DstBuf, size_t ElementSize, size_t Count, FILE *File) {
	size_t ret;

	if (EUR_PC_FP == File && ElementSize == 1 && Count == 0x00017700) {
		//printf("File currunt seek:%d\n", ftell(File));
		handle_music_change();
	}

	ret = _ori_fread(DstBuf, ElementSize, Count, File);

	return ret;
}
//

void srand_wrapper(unsigned int seed) {
	printf("ignored seed:%d\n", seed);
}

//__declspec(dllexport) BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	HANDLE Curr_handle = GetCurrentProcess();
	unsigned char *to_patch_1 = (unsigned char *)0x0051A284;
	unsigned char *to_patch_2 = (unsigned char *)0x00401001;
	unsigned char *to_patch_3 = (unsigned char *)0x00468135; //keyboard GetDeviceState patch
	//unsigned char *to_patch_4 = (unsigned char *)0x005391CC; //srand patch
	DWORD OldProtect;
	int ret;

	switch(ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:

		//seed the RNG
		srand(time(NULL));

		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
			ms_ori_lib = LoadLibrary("msvcr80.dll");
			ori_dsound_library = LoadLibrary("DSOUND_ori.dll");
			if (!ori_dsound_library || !ms_ori_lib) {
				MessageBox(NULL, "Error: loading one of the dll files!", "Err", MB_OK | MB_ICONERROR);
				return FALSE;
			}

			_ori_fopen = (fopen_typedef)GetProcAddress(ms_ori_lib, "fopen");
			_ori_fread = (fread_typedef)GetProcAddress(ms_ori_lib, "fread");
			_ori_fseek = (fseek_typedef)GetProcAddress(ms_ori_lib, "fseek");
			_ori_DirectSoundCreate8 = (DirectSoundCreate8_typedef)GetProcAddress(ori_dsound_library, "DirectSoundCreate8");
			if (!_ori_DirectSoundCreate8 | !_ori_fseek | !_ori_fread | !_ori_fopen) {
				MessageBox(NULL, "Error: Getting one of the functions!", "Err", MB_OK | MB_ICONERROR);
				return FALSE;
			}

			if (VirtualProtectEx(Curr_handle, (void *)0x0051A280, 16, PAGE_EXECUTE_READWRITE, &OldProtect) == 0) {
				printf("Err in VirtualProtectEx\n");
				return FALSE;
			}

			if (VirtualProtectEx(Curr_handle, (void *)0x00401000, 16, PAGE_EXECUTE_READWRITE, &OldProtect) == 0) {
				printf("Err in VirtualProtectEx 2\n");
				return FALSE;
			}
			VirtualProtectEx(Curr_handle, (void *)0x0053919C, 4, PAGE_EXECUTE_READWRITE, &OldProtect);
			VirtualProtectEx(Curr_handle, (void *)0x005391C4, 4, PAGE_EXECUTE_READWRITE, &OldProtect);
			VirtualProtectEx(Curr_handle, (void *)0x005391BC, 4, PAGE_EXECUTE_READWRITE, &OldProtect);
			*((unsigned int *)(0x0053919C)) = (unsigned int *)(&fopen_wrapper);
			*((unsigned int *)(0x005391C4)) = (unsigned int *)(&fread_wrapper);
			*((unsigned int *)(0x005391BC)) = (unsigned int *)(&fseek_wrapper);

			to_patch_1[0] = 0xE8;
			to_patch_1[1] = 0x7C;
			to_patch_1[2] = 0x6D;
			to_patch_1[3] = 0xEE;
			to_patch_1[4] = 0xFF;

			to_patch_2[0] = 0x90;
			to_patch_2[1] = 0x90;
			to_patch_2[2] = 0x90;
			to_patch_2[3] = 0x90;
			to_patch_2[4] = 0x90;
			to_patch_2[5] = 0xBD;
			*((unsigned int *)(to_patch_2+6)) = (unsigned int *)( (&music_handler) );
			to_patch_2[10] = 0xFF;
			to_patch_2[11] = 0xE5;

			MessageBox(NULL, "Patched!", "MainDll", MB_OK | MB_ICONINFORMATION);
			break;

		case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
			break;
		case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
			break;
		case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
			break;
    }

    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}