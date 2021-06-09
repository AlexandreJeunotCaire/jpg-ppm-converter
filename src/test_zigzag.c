#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "zigzag.h"
#include "matrix.h"

#if 0

int main()
{
	int16_t *input = calloc(sizeof(int16_t), 64);
	int16_t *output = calloc(sizeof(int16_t), 64);

	srand(time(NULL));
	for (uint8_t i = 0; i < 64; i++) {
		input[i] = rand();
	}

	printf("Entrée zigzag:\n");
	print_block_int16(input);

	zigzag(input, output);

	printf("Sortie zigzag:\n");
	print_vector_int16(output, 64);

	free(input);
	free(output);
	return 0;
}

#endif
