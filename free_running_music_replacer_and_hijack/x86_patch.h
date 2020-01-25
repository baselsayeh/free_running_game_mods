#ifndef __x86_h__
#define __x86_h__

BOOL x86_patch_call(HANDLE handle_to_process, uint32_t memory_loc, uint32_t new_loc);
BOOL x86_patch_5bytes(HANDLE handle_to_process, uint32_t memory_loc, uint8_t opcode, uint32_t value);
BOOL x86_patch_generic_4bytes(HANDLE handle_to_process, uint32_t memory_loc, uint32_t new_value);

#endif