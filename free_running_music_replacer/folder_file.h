#include <dirent.h>
#include <string.h>

int file_to_mem(char *fname, unsigned char **out, unsigned long *len) {
	FILE *fp;
	unsigned char *file_malloc;
	long file_size;

	fp = fopen(fname, "rb");
	if (fp == NULL)
		return -1;

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET); /* same as rewind(f); */
	if (file_size == 0 || file_size > 0x4000000) { //64 meg
		fclose(fp);
		return -2;
	}

	file_malloc = (unsigned char *)malloc(file_size + 1);
	if (file_malloc == NULL) {
		fclose(fp);
		return -3;
	}

	if (fread(file_malloc, 1, file_size, fp) != file_size) {
		fclose(fp);
		free(file_malloc);
		return -4;
	}

	fclose(fp);

	*out = file_malloc;
	*len = file_size;
	return 0;
}

