#include <stdint.h>
#include <math.h>

#include "block.h"

struct blocks *cut_blocks(uint8_t array[], size_t width, size_t height)
{
	// Division par 8, arrondi supérieur
	size_t block_width = width >> 3;
	if (width % 8 != 0)
		block_width++;

	// Division par 8, arrondi supérieur
	size_t block_height = height >> 3;
	if (height % 8 != 0)
		block_height++;

	// Allocation
	struct blocks *blocks = malloc(sizeof(struct blocks));
	blocks->width = block_width;
	blocks->height = block_height;
	blocks->array = calloc(block_width * block_height * 64, sizeof(uint8_t));

	size_t index = 0;
	// Pour chaque bloc, ordonné selon index_block = block_y * block_width + block_x
	for (size_t block_y = 0; block_y < block_height; block_y++) {
		for (size_t block_x = 0; block_x < block_width; block_x++) {
			// Pour chaque pixel dans le bloc
			for (uint8_t y = 0; y < 8; y++) {
				for (uint8_t x = 0; x < 8; x++) {
					// On calcule les coordonnées du pixel à copier
					size_t tx = (block_x << 3) + x;
					if (tx >= width)
						tx = width - 1;

					size_t ty = (block_y << 3) + y;
					if (ty >= height)
						ty = height - 1;

					// On copie le pixel
					blocks->array[index++] = array[ty * width + tx];
				}
			}
		}
	}

	return blocks;
}
