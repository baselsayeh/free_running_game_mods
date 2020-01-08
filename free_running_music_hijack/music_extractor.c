#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int32_t table_from_exe_1[] = {
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0x00000002, 0x00000004, 0x00000006, 0x00000008,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0x00000002, 0x00000004, 0x00000006, 0x00000008
};

int32_t table_from_exe_2[] = {
	0x00000007, 0x00000008, 0x00000009, 0x0000000a,
	0x0000000b, 0x0000000c, 0x0000000d, 0x0000000e,
	0x00000010, 0x00000011, 0x00000013, 0x00000015,
	0x00000017, 0x00000019, 0x0000001c, 0x0000001f,
	0x00000022, 0x00000025, 0x00000029, 0x0000002d,
	0x00000032, 0x00000037, 0x0000003c, 0x00000042,
	0x00000049, 0x00000050, 0x00000058, 0x00000061,
	0x0000006b, 0x00000076, 0x00000082, 0x0000008f,
	0x0000009d, 0x000000ad, 0x000000be, 0x000000d1,
	0x000000e6, 0x000000fd, 0x00000117, 0x00000133,
	0x00000151, 0x00000173, 0x00000198, 0x000001c1,
	0x000001ee, 0x00000220, 0x00000256, 0x00000292,
	0x000002d4, 0x0000031c, 0x0000036c, 0x000003c3,
	0x00000424, 0x0000048e, 0x00000502, 0x00000583,
	0x00000610, 0x000006ab, 0x00000756, 0x00000812,
	0x000008e0, 0x000009c3, 0x00000abd, 0x00000bd0,
	0x00000cff, 0x00000e4c, 0x00000fba, 0x0000114c,
	0x00001307, 0x000014ee, 0x00001706, 0x00001954,
	0x00001bdc, 0x00001ea5, 0x000021b6, 0x00002515,
	0x000028ca, 0x00002cdf, 0x0000315b, 0x0000364b,
	0x00003bb9, 0x000041b2, 0x00004844, 0x00004f7e,
	0x00005771, 0x0000602f, 0x000069ce, 0x00007462,
	0x00007fff
};

int32_t char_sign_extend(unsigned char chr) {
	if (chr & 0x80)
		return (0xFFFFFF00 | chr);
	else
		return chr;
}

struct initial_values {
	int32_t ebp;
	int32_t edi;
	int32_t ecx;
	int32_t esi;
};

void decrypt(unsigned char *from, unsigned short *to, int32_t len, struct initial_values *i_v) {
	int32_t arg0, arg2, var8;
	uint16_t *curr_addr_to;
	uint8_t *curr_addr_from;
	int32_t eax;
	int32_t ebx; //not used
	int32_t ecx;
	int32_t edx;
	int32_t esi;
	int32_t edi;
	int32_t ebp;

	var8 = arg2 = arg0 = eax = ebx = ecx = edx = esi = edi = ebp = 0x00;
	curr_addr_to = to;
	curr_addr_from = from;

	//edx = curr_addr_from;
	esi = 0;
	esi = 0;

	if (i_v == NULL) {
		ebp = edi = ecx = esi = 0;
	} else {
		ebp = i_v->ebp;
		edi = i_v->edi;
		ecx = i_v->ecx;
		esi = i_v->esi;
	}

	arg2 = ecx;
	ecx = table_from_exe_2[edi];
	arg0 = ecx;
	ecx = table_from_exe_2[esi];
	var8 = ecx;

	//printf("0051A106: %08x %08x %08x %08x %08x %08x %08x\n", eax, ebx, ecx, edx, esi, edi, ebp);

#ifdef DEBUG
	printf("%08x %08x\n", arg0, var8);
#endif

	//ebx = curr_addr_to;

	do {
		/*eax = curr_addr_from;
		edx = char_sign_extend(*(unsigned char *)eax);
		eax += 1;
		curr_addr_from = (unsigned char *)eax;*/
		edx = curr_addr_from[0];
		curr_addr_from++; //next address

		eax = edx;
		eax >>= 4;
		eax &= 0xF;

		edi += table_from_exe_1[eax];
#ifdef DEBUG
		printf("0051A12D: %08x %08x %08x %08x %08x %08x %08x\n", eax, ebx, ecx, edx, esi, edi, ebp);
#endif

		if (edi > 0) { //Jump Not Signed
			if (edi > 0x58)
				edi = 0x58;
		} else {
			edi = 0;
		}
#ifdef DEBUG
		printf("0051A13D: %08x %08x %08x %08x %08x %08x %08x\n", eax, ebx, ecx, edx, esi, edi, ebp);
#endif

		ecx = eax;
		ecx &= 0x7;
		ecx = ecx * arg0;
		ecx >>= 2;
#ifdef DEBUG
		printf("0051A14C: %08x %08x %08x %08x %08x %08x %08x\n", eax, ebx, ecx, edx, esi, edi, ebp);
#endif

		if (eax & 0x08) {
			ebp -= ecx;
		} else {
			ebp += ecx;
		}
#ifdef DEBUG
		printf("0051A15A: %08x %08x %08x %08x %08x %08x %08x\n", eax, ebx, ecx, edx, esi, edi, ebp);
#endif

		if (ebp <= 0x7FFF) {
			if (ebp < (int32_t)0xFFFF8000)
				ebp = (int32_t)0xFFFF8000;
		} else {
			ebp = 0x7FFF;
		}
#ifdef DEBUG
		printf("0051A170: %08x %08x %08x %08x %08x %08x %08x\n", eax, ebx, ecx, edx, esi, edi, ebp);
#endif

		eax = table_from_exe_2[edi];

		//*((unsigned short *)ebx) = (ebp & 0xFFFF);
		edx &= 0x0F;
		//ebx += 2;
		curr_addr_to[0] = (ebp & 0xFFFF);
		curr_addr_to++; //next address

		esi += table_from_exe_1[edx];
		arg0 = eax;
#ifdef DEBUG
		printf("0051A18B: %08x %08x %08x %08x %08x %08x %08x\n", eax, ebx, ecx, edx, esi, edi, ebp);
#endif

		if (esi < 0) {
			esi = 0;
		} else if (esi > 0x58) {
			esi = 0x58;
		}

		eax = edx;
		eax &= 0x07;
		eax = eax * var8;
		eax >>= 2;
#ifdef DEBUG
		printf("0051A1AB: %08x %08x %08x %08x %08x %08x %08x\n", eax, ebx, ecx, edx, esi, edi, ebp);
#endif

		if (edx & 0x08) {
			arg2 -= eax;
		} else {
			arg2 += eax;
		}

		eax = arg2;

		if (eax <= 0x7FFF) {
			if (eax < (int32_t)0xFFFF8000)
				arg2 = (int32_t)0xFFFF8000;
		} else {
			arg2 = 0x7FFF;
		}

		ecx = table_from_exe_2[esi];
		edx = (arg2 & 0xFFFF);
		len--;
		/**((unsigned short *)ebx) = (edx & 0xFFFF);
		ebx += 2;*/
		curr_addr_to[0] = (edx & 0xFFFF);
		curr_addr_to++; //next address

		var8 = ecx;

#ifdef DEBUG
		printf("%08x %08x %08x %08x %08x %08x %08x\n\n", eax, ebx, ecx, edx, esi, edi, ebp);
#endif
	} while (len > 0);

	eax = arg2;
	printf("These values is used if there is need to decode a several chunks\n%d %d %d %d\n", ebp, edi, ecx, esi);
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
	if (file_size == 0 || file_size > 0x2000000) { //32 meg
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
		return -3;
	}

	fclose(fp);

	*out = file_malloc;
	*len = file_size;
	return 0;
}

int main(int argc, char *argv[]) {
	unsigned long file_length;
	unsigned char *file_in_mem;
	FILE *fp_write;
	short *out_dec;
	int ret;
	char *out_as_char;
	struct initial_values init_vals;

	if (argc != 3 && argc != 7) {
		printf("Usage: %s input_file output_file\nOr: %s input_file output_file 4values", argv[0]);
		return 1;
	}

	//printf("Args: %d", argc);
	if (argc == 7) {
		sscanf(argv[3], "%d", &(init_vals.ebp));
		sscanf(argv[4], "%d", &(init_vals.edi));
		sscanf(argv[5], "%d", &(init_vals.ecx));
		sscanf(argv[6], "%d", &(init_vals.esi));
	}

	ret = file_to_mem(argv[1], &file_in_mem, &file_length);
	if (ret != 0) {
		printf("Error reading file: %d\n", ret);
		return 1;
	}

	out_dec = (short *)malloc(file_length * 4);
	//                                    2*2: 16 bit and 2 channels
	if (out_dec == NULL) {
		printf("Error allocating %d bytes", file_length * 4);
		return 1;
	}
	out_as_char = (char *)out_dec;

	if (argc == 7)
		decrypt(file_in_mem, out_dec, file_length, &init_vals);
	else
		//decrypt(file_in_mem, out_dec, 0x5DC0, NULL);
		decrypt(file_in_mem, out_dec, file_length, NULL);

	free(file_in_mem);

	fp_write = fopen(argv[2], "wb");
	if (fp_write == NULL) {
		printf("Error opening the output file!\n");
		free(out_dec);
		return 1;
	}
	//for (int i=0; i<file_length; i++) {
	for (int i=0; i<(file_length*4); i++) {
		fwrite(&out_as_char[i], 1, 1, fp_write);
	}
	fclose(fp_write);

	free(out_dec);

	return 0;
}
