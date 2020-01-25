
//int file_to_mem(char *fname, unsigned char **out, unsigned long *len);

unsigned long file_length = 0;
unsigned char *file_in_mem = NULL;
unsigned long curr_file_offset = 0;


//needed for the player
FILE *EUR_PC_FP = NULL;

int last_Offset = 0;
int music_change = 0;
int curr_music = 0;

////int lock_music_controlls = 0;

volatile int music_can_handle = 0;

volatile int music_pause = 0;


#include <bcrypt.h>

#ifdef MUSIC_HIJACK
unsigned int really_get_random_num() {
	int num_ret = 0;

	if (BCryptGenRandom(NULL, (PUCHAR)&num_ret, sizeof(num_ret), BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0) {
		printf("Random number: cannot get from BCryptGenRandom(), falling back to rand()\n");
		num_ret = rand();
	}

	return (unsigned int)num_ret;
}
#endif

/*
#MUSIC OFFSET
512CD7         //add     [esi+3Ch], ecx #every time
5128AE         //mov     [esi+3Ch], eax #when music changes
512CF7         //mov     ecx, [esi+3Ch] #does not change when in main menu (changes in music menu in hangout or in game)

#FILE OFFSET
512B07         //mov     ecx, [esi+58h] #every time
512C33         //add     [esi+58h], eax #every time
512C36         //mov     eax, [esi+58h] #every time

5128B6         // #when the music change only
*/
unsigned int *curr_file_offset_music = NULL;
unsigned int *curr_music_pos = NULL;

//unsigned int ori_curr_file_offset_music = 0;

void ger_ptr_curr_music_fileoffset() {
#ifndef MUSIC_HIJACK //does not work for some reason
	unsigned int ptr1 = *(unsigned int *) (freerunning_base_address + 0x00191458);
	unsigned int ptr2 = *(unsigned int *) (ptr1 + 0x24);
	unsigned int ptr3 = *(unsigned int *) (ptr2 + 0x6FC);
	unsigned int ptr4 = *(unsigned int *) (ptr3 + 0xAC);
	unsigned int ptr5 = *(unsigned int *) (ptr4 + 0x0);
	unsigned int ptr6 = *(unsigned int *) (ptr5 + 0x11C); //0x12263FA8

	unsigned int ptr7 = *(unsigned int *) (ptr6 + 0xFC);
	unsigned int *ptr8 = (unsigned int *) (ptr7 + 0xF8);

	curr_file_offset_music = ptr8;
#endif

	return;
}

void ger_ptr_curr_music_pos() {
#ifndef MUSIC_HIJACK //does not work for some reason
	unsigned int ptr1 = *(unsigned int *) (freerunning_base_address + 0x001A0128);
	unsigned int ptr2 = *(unsigned int *) (ptr1 + 0x110);
	unsigned int ptr3 = *(unsigned int *) (ptr2 + 0x11C);
	unsigned int ptr4 = *(unsigned int *) (ptr3 + 0x260);
	unsigned int ptr5 = *(unsigned int *) (ptr4 + 0x4);
	unsigned int ptr6 = *(unsigned int *) (ptr5 + 0xFC);
	unsigned int ptr7 = *(unsigned int *) (ptr6 + 0x0);

	unsigned int *ptr8 = (unsigned int *) (ptr7 + 0x3C); //

	curr_music_pos = ptr8;
#endif

	return;
}

void handle_music_change() {
	if (music_can_handle == 2)
		return;
	switch (last_Offset) {
		case 0x0bf95b28:
			curr_music = 1;
			break;
		case 0x03158958:
			curr_music = 2;
			break;
		case 0x075f28b0:
			curr_music = 3;
			break;
		case 0x05266ca8:
			curr_music = 4;
			break;
		case 0x0b755b28:
			curr_music = 5;
			break;
		case 0x00e52830:
			curr_music = 6;
			break;
		case 0x0dbdccc8:
			curr_music = 7;
			break;
		case 0x05a5cca8:
			curr_music = 8;
			break;
		case 0x07d018f0:
			curr_music = 9;
			break;
		case 0x03c5f758:
			curr_music = 10;
			break;
		case 0x0af74558:
			curr_music = 11;
			break;
		case 0x087047f0:
			curr_music = 12;
			break;
		case 0x0d2c3648:
			curr_music = 13;
			break;
		case 0x02300230:
			curr_music = 14;
			break;
		case 0x064be5a8:
			curr_music = 15;
			break;
		case 0x0c54df48:
			curr_music = 16;
			break;
		case 0x0a7dfa70:
			curr_music = 17;
			break;
		case 0x01768430:
			curr_music = 18;
			break;
		case 0x0cb95648:
			curr_music = 19;
			break;
		case 0x091e9a70:
			curr_music = 20;
			break;
		case 0x06bb4470:
			curr_music = 21;
			break;
		case 0x09d3aa70:
			curr_music = 22;
			break;
		case 0x048b7060:
			curr_music = 23;
			break;
		default:
			music_can_handle = 0;
			return;
			break;
	}
	music_change = 1;
#ifndef MUSIC_HIJACK
	curr_file_offset = 0;
	music_can_handle = 1;
#endif
	ger_ptr_curr_music_fileoffset();
	ger_ptr_curr_music_pos();
	//ori_curr_file_offset_music = (*curr_file_offset_music)+0x17700;
}

void free_music_mem() {
	if (file_in_mem != NULL)
		free(file_in_mem);
	file_in_mem = NULL;

	return;
}

void music_finish_go_to_next() {
	char *to_read_f_name = NULL;
	char f_name[512] = {0};
#ifdef MUSIC_HIJACK
	unsigned int index = (really_get_random_num() % (ll_count(&ll_head)));
	char out_f_name[512] = {0};
#endif

	if (music_can_handle == 0) {
		if (curr_music_pos != NULL)
			*curr_music_pos = 0x7FF00000;
		return;
	}

	music_pause = 1;

	free_music_mem();

#ifdef MUSIC_HIJACK
	if (ll_get_index(index, &ll_head, f_name) != 0) {
		printf("Error getting file %d!\n", index);
		music_can_handle = 0;
		goto end;
	}

	snprintf(out_f_name, sizeof(f_name), "Music\\%s", f_name);

	to_read_f_name = out_f_name;
#else
	snprintf(f_name, sizeof(f_name), "Music\\%d.raw", curr_music);

	to_read_f_name = f_name;
#endif

	if (access(to_read_f_name, R_OK) == -1) {
		printf("file %s is NOT found or NOT readable!\n", to_read_f_name);
		printf("Falling back to default music player!\n");
		music_can_handle = 0;
		goto end;
	}

	printf("Will try to play %s\n", to_read_f_name);

	if (file_to_mem(to_read_f_name, &file_in_mem, &file_length) != 0) {
		printf("Error reading file %s\n", strerror(errno));
		music_can_handle = 0;
		goto end;
	}

end:
	curr_file_offset = 0;
	music_pause = 0;
}

#ifdef MUSIC_HIJACK
void set_music_can_handle_from_keyboard(int new_val) {
	int old_value = music_can_handle;
	music_can_handle = new_val;

	if (old_value != 0 && new_val == 0) {
		/*if (file_in_mem != NULL)
			free(file_in_mem);
		file_in_mem = NULL;*/

		//*curr_music_pos = 0x7FF00000;
		curr_file_offset = 0;
		music_pause = 1;
		free_music_mem();
		music_pause = 0;
	}

	if (old_value == 0 && new_val == 2) {
		music_pause = 1;
		music_finish_go_to_next();
		music_pause = 0;
	}
	//usleep(10e3);
	Sleep(10);

#ifdef DEBUG
	printf("DBG: music_can_handle %d\n", music_can_handle);
#endif
}
#endif

void __stdcall music_handler(char *from, char *to, int len, char *ignore) {
	int length_to_process = (len*4);

	if (music_change == 1 && music_can_handle != 2) { //music did change
		memset((void *)to, 0x00, length_to_process);
		music_finish_go_to_next();
	}
	music_change = 0;

#ifdef DEBUG
	if (curr_file_offset_music != NULL) {
		printf("DBG: Curr FILE offset: %d\n", *curr_file_offset_music);
		printf("DBG: Curr MUSC offset: %d\n", *curr_music_pos);
	}
#endif

	if (music_pause == 0) {
		if (music_can_handle == 1 || music_can_handle == 2) {
				//
				if (length_to_process > (file_length - curr_file_offset)) {
					length_to_process = (file_length - curr_file_offset);
				}
				//*curr_file_offset_music = ori_curr_file_offset_music; //this will break the game
				if (curr_music_pos != NULL)
					*curr_music_pos = 24000;

				if (file_in_mem != NULL) {
					memcpy((void *)to, (void *)(file_in_mem + curr_file_offset), length_to_process);
				} else {
					printf("Warning: file_in_mem IS NULL!\n");
					memset((void *)to, 0x00, length_to_process);
					return;
				}
				curr_file_offset += length_to_process;
				if (curr_file_offset >= file_length) {
					music_finish_go_to_next();
					if (curr_music_pos != NULL)
						*curr_music_pos = 0x7FF00000;
					//curr_file_offset = 0;
				}
			//printf("A: Music handler to %d %d %d!\n\n", curr_file_offset, len, file_length);
		} else {
			//printf("Cant handle music %d\n\n\n", curr_music);
			ori_decrypt_music(from, to, len, ignore);
		}
	} else {
		memset((void *)to, 0x00, length_to_process);
	}
}