#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <DSound.h>

typedef HRESULT WINAPI (*DirectSoundCreate8_typedef)(LPCGUID lpcGuidDevice, LPDIRECTSOUND8 * ppDS8, LPUNKNOWN pUnkOuter);

HINSTANCE ori_library;
FILE *fp;
DirectSoundCreate8_typedef _ori_DirectSoundCreate8;


HRESULT WINAPI DirectSoundCreate8(LPCGUID lpcGuidDevice, LPDIRECTSOUND8 * ppDS8, LPUNKNOWN pUnkOuter) {
	return _ori_DirectSoundCreate8(lpcGuidDevice, ppDS8, pUnkOuter);
}


void test_func(unsigned int *p1, ...) {
	unsigned int vars[11];
	unsigned int *vars_to_be_passed = NULL;
	unsigned int *format_to_be_passed = NULL;
	unsigned char *format_to_be_passed_as_char = NULL;
	va_list vl;

	va_start(vl, 11);
	for (int i=0; i<11; i++) {
		vars[i] = va_arg(vl, unsigned int);
	}
	va_end(vl);

	if ( (((int) p1)>>20) != 0x0005) {
		if ( (vars[0]>>20) != 0x0005) {
			goto ended; //string not found!
		} else {
			vars_to_be_passed = (&vars[1]);
			format_to_be_passed = (unsigned int *)vars[0];
			//printf("2\n");
		}
	} else {
		vars_to_be_passed = (&vars[0]);
		format_to_be_passed = p1;
		//printf("1\n");
	}

	format_to_be_passed_as_char = (unsigned char *)format_to_be_passed;
	/*if (format_to_be_passed == 0x0054d6f8) {
		printf("R\n");
		goto ended;
	}*/
	if (fp != NULL) {
		fprintf(fp, format_to_be_passed_as_char, vars_to_be_passed[0], vars_to_be_passed[1], vars_to_be_passed[2], vars_to_be_passed[3],
										vars_to_be_passed[4], vars_to_be_passed[5], vars_to_be_passed[6], vars_to_be_passed[7],
										vars_to_be_passed[8], vars_to_be_passed[9]);
		
	}
	//printf(format_to_be_passed, vars_to_be_passed[0], vars_to_be_passed[1], vars_to_be_passed[2], vars_to_be_passed[3]);

	//printf("Got %08x\n", ptr);
ended:
	return;
	//printf("Testfunc!\n");
}

//__declspec(dllexport) BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	HANDLE Curr_handle = GetCurrentProcess();
	unsigned char *to_patch = (unsigned char *)0x00401000;
	DWORD OldProtect;

	switch(ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
			ori_library = LoadLibrary("DSOUND_ori.dll");
			if (!ori_library) {
				MessageBox(NULL, "Error: loading!", "Err", MB_OK | MB_ICONINFORMATION);
				return FALSE;
			}

			_ori_DirectSoundCreate8 = (DirectSoundCreate8_typedef)GetProcAddress(ori_library, "DirectSoundCreate8");
			if (!_ori_DirectSoundCreate8) {
				MessageBox(NULL, "Error: Getting!", "Err", MB_OK | MB_ICONINFORMATION);
				return FALSE;
			}

			fp = fopen("log.txt", "wb");
			if (fp == NULL) {
				MessageBox(NULL, "Error opening file!", "Err", MB_OK | MB_ICONINFORMATION);
				return FALSE;
			}

			if (VirtualProtectEx(Curr_handle, (void *)0x00401000, 16, PAGE_EXECUTE_READWRITE, &OldProtect) == 0) {
				printf("Err in VirtualProtectEx\n");
				return FALSE;
			}

			to_patch[0] = 0x68;
			*((unsigned int *)(to_patch+1)) = (unsigned int *)(&test_func);
			to_patch[5] = 0xC3;

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
			if (fp != NULL)
				fclose(fp);
			break;
    }

    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
