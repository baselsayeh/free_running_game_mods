
//int file_to_mem(char *fname, unsigned char **out, unsigned long *len);

/*
int last_Offset;
int music_change;
int curr_music;

int lock_music_controlls;

int music_can_handle;
*/

#include <bcrypt.h>

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
		default:
			music_can_handle = 0;
			return;
			break;
	}
	music_change = 1;
	curr_file_offset = 0;
	music_can_handle = 1;
}

void music_finish_go_to_next() {
	char f_name[64] = {0};

	if (file_in_mem != NULL)
		free(file_in_mem);
	file_in_mem = NULL;

	sprintf(f_name, "Music\\%d.raw", curr_music);

	if (file_to_mem(f_name, &file_in_mem, &file_length) != 0) {
		/*printf("Error reading file %s\n", strerror(errno));
		exit(1);*/
		music_can_handle = 0;
	} else {
		printf("Will play %s\n", f_name);
	}

	curr_file_offset = 0;
	music_pause = 0;
}

void __stdcall music_handler(char *from, char *to, int len, char *ignore) {
	int length_to_process = (len*4);

	if (music_change == 1) { //music did change
		memset((void *)to, 0x00, length_to_process);
		music_finish_go_to_next();
	}
	music_change = 0;

	if (music_can_handle == 1) {
		if (music_pause == 0) {
			if (length_to_process > (file_length - curr_file_offset)) {
				length_to_process = (file_length - curr_file_offset);
			}
			memcpy((void *)to, (void *)(file_in_mem + curr_file_offset), length_to_process);
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