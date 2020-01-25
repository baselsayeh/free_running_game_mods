#include <stdio.h>
#include <stdint.h>
#include <winsock2.h>
#include <windows.h>
#include <time.h>
#include <DSound.h>
#include <errno.h>
//#include "sleep_a.h"
#include "folder_file.h"
#include "x86_patch.h"

//the types of the original functions
typedef __stdcall HRESULT WINAPI (*DirectSoundCreate8_typedef)(LPCGUID lpcGuidDevice, LPDIRECTSOUND8 * ppDS8, LPUNKNOWN pUnkOuter);
typedef FILE *(*fopen_typedef)(const char *filename, const char *mode);
typedef size_t (*fread_typedef)(void *DstBuf, size_t ElementSize, size_t Count, FILE *File);
typedef int (*fseek_typedef)(FILE *File, int Offset, int Origin);
typedef __stdcall void (*decrypt_music_typedef)(unsigned char *from, unsigned char *to, int len, char *i_v);
#ifdef MUSIC_HIJACK
typedef __stdcall int (*dinput8_GetDeviceState_typedef)(void *this, int size, void *to);
#endif
//

//
//variables

HINSTANCE ori_dsound_library;
#ifdef MUSIC_HIJACK
HINSTANCE ori_dinput8_library;
#endif
DirectSoundCreate8_typedef _ori_DirectSoundCreate8;
fopen_typedef _ori_fopen;
fread_typedef _ori_fread;
fseek_typedef _ori_fseek;
decrypt_music_typedef ori_decrypt_music;
//needed to get the original addresses for the fopen,fseek and fread functions
HINSTANCE ms_ori_lib;
#ifdef MUSIC_HIJACK
HANDLE keybd_thread; //needed to monitor the keyboard

dinput8_GetDeviceState_typedef ori_dinput8_GetDeviceState;
#endif

unsigned int freerunning_base_address = 0;

#ifdef MUSIC_HIJACK
struct link_list_char_array ll_head;
#endif
//

//another includes
#include "music_controls.h"
#ifdef MUSIC_HIJACK
#include "keyboard_hook.h"
#endif
//
///////////////////////////////////////

//dll functions
HRESULT WINAPI DirectSoundCreate8(LPCGUID lpcGuidDevice, LPDIRECTSOUND8 * ppDS8, LPUNKNOWN pUnkOuter) {
	return _ori_DirectSoundCreate8(lpcGuidDevice, ppDS8, pUnkOuter);
}

#ifdef MUSIC_HIJACK
int __stdcall DETECT_INPUT(void *this, void *to) {
	int ret = 0;
	ret = ori_dinput8_GetDeviceState(this, 0x100, to);
	if (keyboard_updated == 0) //no need to update if keys is not processed
		memcpy(m_keyboardState, to, 0xFF);
	keyboard_updated = 1;
	return ret;
}
#endif


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

/*void srand_wrapper(unsigned int seed) {
	printf("ignored seed:%d\n", seed);
}*/

//__declspec(dllexport) BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	HANDLE Curr_handle = GetCurrentProcess();
	freerunning_base_address = (unsigned int) GetModuleHandleA("FreeRunning.exe"); //get the base address of FreeRunning.exe (must be 0x400000)
	int ret;
	unsigned int error_patch = 0;

	if (freerunning_base_address == 0) { //null
		MessageBox(NULL, "Error: getting the base addresses of FreeRunning.exe!", "Err", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	ori_decrypt_music = (decrypt_music_typedef)(freerunning_base_address + 0x11A0C0);

	switch(ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:

		//seed the RNG
		srand(time(NULL));

#ifdef MUSIC_HIJACK
		//list the folder Music
		list_folder_to_ll("Music", &ll_head);
		if (ll_count(&ll_head) == 0 || ll_count(&ll_head) > 20) {
			printf("No files or too many files!\n");
			return FALSE;
		}
		ll_print(&ll_head);
		printf("--------------------\n\n");
		//
#endif

#ifdef MUSIC_HIJACK
		//initialize the keyboard thread
		keybd_thread = CreateThread(NULL, 0, monitor_keyboard_thread, NULL, 0, NULL);
		//
#endif

		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
			ms_ori_lib = LoadLibrary("msvcr80.dll");
			ori_dsound_library = LoadLibrary("DSOUND_ori.dll");
#ifdef MUSIC_HIJACK
			ori_dinput8_library = LoadLibrary("dinput8.dll");
#endif

			if (!ori_dsound_library || !ms_ori_lib) {
				MessageBox(NULL, "Error: loading one of the dll files!", "Err", MB_OK | MB_ICONERROR);
				return FALSE;
			}

#ifdef MUSIC_HIJACK
			if (!ori_dinput8_library) {
				MessageBox(NULL, "Error: loading dinput8 dll!", "Err", MB_OK | MB_ICONERROR);
				return FALSE;
			}
#endif

			_ori_fopen = (fopen_typedef)GetProcAddress(ms_ori_lib, "fopen");
			_ori_fread = (fread_typedef)GetProcAddress(ms_ori_lib, "fread");
			_ori_fseek = (fseek_typedef)GetProcAddress(ms_ori_lib, "fseek");
			_ori_DirectSoundCreate8 = (DirectSoundCreate8_typedef)GetProcAddress(ori_dsound_library, "DirectSoundCreate8");

#ifdef MUSIC_HIJACK
			ori_dinput8_GetDeviceState = (dinput8_GetDeviceState_typedef)GetProcAddress(ori_dinput8_library, "GetdfDIJoystick");
#endif

			if (!_ori_DirectSoundCreate8 | !_ori_fseek | !_ori_fread | !_ori_fopen) {
				MessageBox(NULL, "Error: Getting one of the functions!", "Err", MB_OK | MB_ICONERROR);
				return FALSE;
			}

#ifdef MUSIC_HIJACK
			if (!ori_dinput8_GetDeviceState) {
				MessageBox(NULL, "Error: Getting GetdfDIJoystick from dinput8.dll!", "Err", MB_OK | MB_ICONERROR);
				return FALSE;
			}
			ori_dinput8_GetDeviceState += 0x27A0; //Get device state have 0x27A0 offset from the exported function GetdfDIJoystick
#endif

			if (x86_patch_generic_4bytes(Curr_handle, freerunning_base_address + 0x13919C, (uint32_t) &fopen_wrapper) == FALSE) {
				error_patch = 0x13919C;
				goto error_patching;
			}
			if (x86_patch_generic_4bytes(Curr_handle, freerunning_base_address + 0x1391C4, (uint32_t) &fread_wrapper) == FALSE) {
				error_patch = 0x1391C4;
				goto error_patching;
			}
			if (x86_patch_generic_4bytes(Curr_handle, freerunning_base_address + 0x1391BC, (uint32_t) &fseek_wrapper) == FALSE) {
				error_patch = 0x1391BC;
				goto error_patching;
			}

			if (x86_patch_call(Curr_handle, freerunning_base_address + 0x11A284, (uint32_t) &music_handler) == FALSE) {
				error_patch = 0x11A284;
				goto error_patching;
			}

#ifdef MUSIC_HIJACK
			if (x86_patch_5bytes(Curr_handle, freerunning_base_address + 0x68135, 0xBA, (uint32_t) &DETECT_INPUT) == FALSE) {
				error_patch = 0x68135;
				goto error_patching;
			}
#endif

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

error_patching:
	printf("Error patching FreeRunning.exe + %08x, err (%d):%s\n", error_patch, errno, strerror(errno));
	MessageBox(NULL, "Error: Patching one of the functions, See the console log!", "Err", MB_OK | MB_ICONERROR);
	return FALSE;

}