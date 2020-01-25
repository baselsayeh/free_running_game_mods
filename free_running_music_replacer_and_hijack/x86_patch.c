#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include <errno.h>

int32_t x86_call_calculate_offset(int32_t old_off, int32_t new_off) {
	return ((new_off-5) - old_off);
}

BOOL x86_patch_call(HANDLE handle_to_process, uint32_t memory_loc, uint32_t new_loc) {
	uint8_t *to_patch = (uint8_t *)memory_loc;
	DWORD OldProtect; //the old protect value

	if (VirtualProtectEx(handle_to_process, (void *)memory_loc, 8, PAGE_EXECUTE_READWRITE, &OldProtect) == FALSE)
		return FALSE;

	to_patch[0] = 0xE8;
	*(int32_t *) (&to_patch[1]) = x86_call_calculate_offset(memory_loc, new_loc);

	VirtualProtectEx(handle_to_process, (void *)memory_loc, 8, OldProtect, &OldProtect); //restore the old protect value

	return TRUE;
}

BOOL x86_patch_5bytes(HANDLE handle_to_process, uint32_t memory_loc, uint8_t opcode, uint32_t value) {
	uint8_t *to_patch = (uint8_t *)memory_loc;
	DWORD OldProtect; //the old protect value

	if (VirtualProtectEx(handle_to_process, (void *)memory_loc, 8, PAGE_EXECUTE_READWRITE, &OldProtect) == FALSE)
		return FALSE;

	to_patch[0] = opcode;
	*(int32_t *) (&to_patch[1]) = value;

	VirtualProtectEx(handle_to_process, (void *)memory_loc, 8, OldProtect, &OldProtect); //restore the old protect value

	return TRUE;
}

BOOL x86_patch_generic_4bytes(HANDLE handle_to_process, uint32_t memory_loc, uint32_t new_value) {
	uint32_t *to_patch = (uint32_t *)memory_loc;
	DWORD OldProtect; //the old protect value

	if (VirtualProtectEx(handle_to_process, (void *)memory_loc, 4, PAGE_EXECUTE_READWRITE, &OldProtect) == FALSE)
		return FALSE;

	to_patch[0] = new_value; //patch the value

	VirtualProtectEx(handle_to_process, (void *)memory_loc, 4, OldProtect, &OldProtect); //restore the old protect value

	return TRUE;
}

