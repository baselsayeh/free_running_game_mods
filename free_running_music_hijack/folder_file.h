#include <dirent.h>
#include <string.h>
#include "LinkList_CharArray.h"

/*int load_file_from_folder_index(int index, char *file_name) {
	int ret = 0;
	DIR *dp = NULL;
	struct dirent *ep = NULL;
	int count = 0;

	dp = opendir(".");
	if(NULL == dp) {
		//fprintf(stderr, "no such directory");
		return 1;
	}

	while (ep = readdir(dp)) {
		if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0) {
			continue;
		}
		if (index == count)
			break;
		count++;
	}
	if (index == count && ep != NULL) {
		sprintf(file_name, "%s", ep->d_name);
		ret = 0;
	} else {
		ret = 2;
		goto finish;
	}

finish:
	closedir(dp);
	return ret;
}*/

int list_folder_to_ll(char *folder_name, struct link_list_char_array *ll_head) {
	int ret = 0;
	DIR *dp = NULL;
	struct dirent *ep = NULL;
	int count = 0;

	dp = opendir(folder_name);
	if(NULL == dp) {
		//fprintf(stderr, "no such directory");
		return 1;
	}

	while (ep = readdir(dp)) {
		if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0) {
			continue;
		}
		ll_add(ll_head, ep->d_name, strlen(ep->d_name));
	}

finish:
	closedir(dp);
	return ret;
}

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

