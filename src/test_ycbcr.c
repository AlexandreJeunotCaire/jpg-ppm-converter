#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ycbcr.h"
#include "matrix.h"

#if 0

int main()
{
	uint8_t *red_y = calloc(sizeof(uint8_t), 64);
	uint8_t *green_cb = calloc(sizeof(uint8_t), 64);
	uint8_t *blue_cr = calloc(sizeof(uint8_t), 64);

	srand(time(NULL));
	for (uint8_t i = 0; i < 64; i++) {
		red_y[i] = rand() % 256;
		green_cb[i] = rand() % 256;
		blue_cr[i] = rand() % 256;
	}

	printf("Red\n");
	print_block_uint8(red_y);
	printf("Green\n");
	print_block_uint8(green_cb);
	printf("Blue\n");
	print_block_uint8(blue_cr);

	rgb_to_ycbcr(red_y, green_cb, blue_cr, 64);

	printf("Y\n");
	print_block_uint8(red_y);
	printf("Cb\n");
	print_block_uint8(green_cb);
	printf("Cr\n");
	print_block_uint8(blue_cr);

	free(red_y);
	free(green_cb);
	free(blue_cr);

	return 0;
}

#endif
