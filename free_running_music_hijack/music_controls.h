
//int file_to_mem(char *fname, unsigned char **out, unsigned long *len);

/*
int last_Offset;
int music_change;
int curr_music;

int lock_music_controlls;

int music_can_handle;
*/

#include <bcrypt.h>

unsigned int really_get_random_num() {
	int num_ret = 0;

	if (BCryptGenRandom(NULL, (PUCHAR)&num_ret, sizeof(num_ret), BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0) {
		printf("Random number: cannot get from BCryptGenRandom(), falling back to rand()\n");
		num_ret = rand();
	}

	return (unsigned int)num_ret;
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
		default:
			return;
			break;
	}
	music_change = 1;
	curr_file_offset = 0;
}

void music_finish_go_to_next() {
	//int index = (rand() % (ll_count(&ll_head)));
	unsigned int index = (really_get_random_num() % (ll_count(&ll_head)));
	char f_name[512] = {0};
	char out_f_name[512] = {0};

	//printf("index: %d\n", index);

	if (ll_get_index(index, &ll_head, f_name) != 0) {
		printf("Error getting file %d\n", index);
		exit(1);
	}

	sprintf(out_f_name, "Music\\%s", f_name);

	if (file_in_mem != NULL)
		free(file_in_mem);

	printf("Will play %s\n", f_name);

	if (file_to_mem(out_f_name, &file_in_mem, &file_length) != 0) {
		printf("Error reading file %s\n", strerror(errno));
		exit(1);
	}

	curr_file_offset = 0;
	music_pause = 0;
}

void __stdcall music_handler(char *from, char *to, int len, char *ignore) {
	int length_to_process = (len*4);

	if (music_change == 1 && music_can_handle != 2) { //music did change
		memset((void *)to, 0x00, length_to_process);
		if (curr_music == 2 && music_can_handle != 2) {
			music_finish_go_to_next();
			music_can_handle = 1;
		} else {
			music_can_handle = 0;
		}
	}
	music_change = 0;

	if (music_can_handle == 1 || music_can_handle == 2) {
		if (music_pause == 0) {
			if (length_to_process > (file_length - curr_file_offset)) {
				length_to_process = (file_length - curr_file_offset);
			}
			memcpy((void *)to, (void *)(file_in_mem + curr_file_offset), length_to_process);
			///memcpy(to, file_in_mem, length_to_process);
			////printf("%d %d %d\n", length_to_process, len, curr_file_offset);

			/*if (length_to_process != len) {
				memset((void *)to, 0x00, len-length_to_process);
			}*/

			//printf("B: Music handler to %d %d %d!\n", curr_file_offset, len, file_length);
			curr_file_offset += length_to_process;
			if (curr_file_offset >= file_length)
				music_finish_go_to_next();
				//curr_file_offset = 0;
		} else {
			memset((void *)to, 0x00, length_to_process);
		}
		//printf("A: Music handler to %d %d %d!\n\n", curr_file_offset, len, file_length);
	} else {
		//printf("Cant handle music %d\n\n\n", curr_music);
		ori_decrypt_music(from, to, len, ignore);
	}
}
