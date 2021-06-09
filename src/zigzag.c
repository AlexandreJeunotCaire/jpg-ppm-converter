#include <stdint.h>

#include "zigzag.h"

// On utilise une table pour éviter les calculs répétés de l'indice.
static uint8_t index_map[64] = {
	0, 1, 5, 6, 14, 15, 27, 28,
	2, 4, 7, 13, 16, 26, 29, 42,
	3, 8, 12, 17, 25, 30, 41, 43,
	9, 11, 18, 24, 31, 40, 44, 53,
	10, 19, 23, 32, 39, 45, 52, 54,
	20, 22, 33, 38, 46, 51, 55, 60,
	21, 34, 37, 47, 50, 56, 59, 61,
	35, 36, 48, 49, 57, 58, 62, 63
};

/**
 * Opération de zigzag.
 * 
 * input: matrice 8x8 en entrée.
 * output: vecteur 64 en sortie. 
 */
void zigzag(int16_t * input, int16_t * output)
{
	for (uint8_t i = 0; i < 64; i++) {
		output[index_map[i]] = input[i];
	}
}
