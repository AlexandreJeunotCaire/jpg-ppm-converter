#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "ppm.h"
#include "matrix.h"
#include "block.h"

#if 0

void print_blocks(uint8_t * array, size_t width, size_t height)
{
	struct blocks *blocks = cut_blocks(array, width, height);

	size_t index = 0;
	for (size_t j = 0; j < blocks->height; j++) {
		for (size_t i = 0; i < blocks->width; i++) {
			printf("Bloc i=%ld j=%ld:\n", i, j);
			print_block_uint8(blocks->array + index);
			index += 64;
		}
	}

	free(blocks->array);
	free(blocks);
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Veuillez spécifier un fichier au format PPM.\n");
		return 0;
	}

	printf("Lecture du fichier ...\n");
	struct ppm *image = read_ppm_file(argv[1]);

	if (!image) {
		printf("Echec.\n");
		return 0;
	}

	printf("Dimensions: %lu %lu\n", image->width, image->height);

	if (image->gray) {
		printf("Gris:\n");
		print_matrix_uint8(image->pixels, image->width, image->height);
	} else {
		size_t size = image->width * image->height * sizeof(uint8_t);
		printf("Rouge:\n");
		print_matrix_uint8(image->pixels, image->width, image->height);
		printf("Vert:\n");
		print_matrix_uint8(image->pixels + size, image->width, image->height);
		printf("Bleu:\n");
		print_matrix_uint8(image->pixels + 2 * size, image->width, image->height);
	}

	printf("\n");
	if (image->gray) {
		printf("Gris:\n");
		print_blocks(image->pixels, image->width, image->height);
	} else {
		size_t size = image->width * image->height * sizeof(uint8_t);
		printf("Rouge:\n");
		print_blocks(image->pixels, image->width, image->height);
		printf("Vert:\n");
		print_blocks(image->pixels + size, image->width, image->height);
		printf("Bleu:\n");
		print_blocks(image->pixels + 2 * size, image->width, image->height);
	}

	free(image->pixels);
	free(image);
}

#endif
