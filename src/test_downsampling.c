#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "ppm.h"
#include "matrix.h"
#include "block.h"
#include "downsampling.h"
#include "ycbcr.h"

#if 0

uint8_t h[3] ={2, 1, 1};
uint8_t v[3] ={2, 1, 1};

//Imprime le résultat du découpage en mcus dans la console
void print_mcu(struct blocks * blocks, uint8_t h, uint8_t v)
{
	struct mcus *mcus = cut_in_mcus(blocks, h, v);

	size_t index = 0;
	for (size_t j = 0; j < mcus->line_nb; j++) {
		for (size_t i = 0; i < mcus->col_nb; i++) {
			printf("mcu i=%ld j=%ld:\n", i, j);
            for(uint8_t block_x=0; block_x < v; block_x++){
                for(uint8_t block_y=0; block_y < h; block_y++){
			        print_block_uint8(mcus->array + index);
			        index += 64;
                }
            }
		}
	}

	free(mcus->array);
	free(mcus);
}


//Imprime le résultat du downsampling sur la console
void print_downsampled(struct mcus *mcus, uint8_t h[], uint8_t v[]){
    size_t index = 0;
    for(uint8_t component=0; component < 3; component++){
        printf("\nAffichage de la composante n°%i\n", component);
        for (size_t j = 0; j < mcus->line_nb; j++) {
	    	for (size_t i = 0; i < mcus->col_nb; i++) {
		    	printf("mcu i=%ld j=%ld:\n", i, j);
                for(uint8_t block_x=0; block_x < v[component]; block_x++){
                    for(uint8_t block_y=0; block_y < h[component]; block_y++){
			            print_block_uint8(mcus->array + index);
			            index += 64;
                    }
                }
		    }
	    }
    }
}

//Fonction principale
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

    struct blocks *red_y = cut_blocks(image->pixels, image->width, image->height);
    struct blocks *green_cb = cut_blocks(image->pixels + image->width * image->height * sizeof(uint8_t), image->width, image->height);
    struct blocks *blue_cr = cut_blocks(image->pixels + 2 * image->width * image->height * sizeof(uint8_t), image->width, image->height);

    rgb_to_ycbcr(red_y->array, green_cb->array, blue_cr->array, red_y->width * red_y->height * 64);

    struct mcus *mcus = downsampling(red_y, green_cb, blue_cr, h, v);

    print_downsampled(mcus, h, v);

	printf("\n");
	free(image->pixels);
	free(image);

    return EXIT_SUCCESS;
}
#endif