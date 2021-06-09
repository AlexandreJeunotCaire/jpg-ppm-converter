#ifndef _BLOCK_H_
#define _BLOCK_H_

#include <stdint.h>

#include "ppm.h"

struct blocks {
	size_t width, height;	// Nombre de blocs
	uint8_t *array;		// Blocs à la suite
};

/**
 * Découpe le tableau donné en blocs de 8x8.
 */
struct blocks *cut_blocks(uint8_t array[], size_t width, size_t height);

#endif				/* _BLOCK_H_ */
