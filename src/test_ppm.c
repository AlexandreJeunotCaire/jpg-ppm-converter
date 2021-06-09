#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "ppm.h"
#include "matrix.h"

#if 0

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
		size_t size = image->width * image->height;
		printf("Rouge:\n");
		print_matrix_uint8(image->pixels, image->width, image->height);
		printf("Vert:\n");
		print_matrix_uint8(image->pixels + size, image->width, image->height);
		printf("Bleu:\n");
		print_matrix_uint8(image->pixels + size * 2, image->width, image->height);
	}

	free(image->pixels);
	free(image);
}

#endif
