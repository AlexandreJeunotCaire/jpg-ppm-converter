#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

#include "dct.h"

#define PI 3.14159265358979323846
#define ISQRT2 0.70710678118

static double cos_cache[8][8];
static bool cos_cache_init = true;

/**
 * Calcul de la transformée en cosinus discrète.
 *
 * input: matrice carrée 8x8 en entrée (spatiale).
 * output: matrice carrée 8x8 en sortie (fréquentielle).
 */
void dct(uint8_t * input, int16_t * output)
{
	// Le cache
	if (cos_cache_init) {
		for (uint8_t i = 0; i < 8; i++) {
			for (uint8_t x = 0; x < 8; x++) {
				cos_cache[i][x] = cos(((double)((2 * x + 1) * i) * PI) / 16.0);
			}
		}

		cos_cache_init = false;
	}

	// Pour chaque (i, j) de la matrice fréquentielle
	for (uint8_t j = 0; j < 8; j++) {
		double cj = j == 0 ? ISQRT2 : 1.0;
		for (uint8_t i = 0; i < 8; i++) {
			double ci = i == 0 ? ISQRT2 : 1.0;

			// On somme pour chaque (x, y) de la matrice spatialle
			double somme = 0;
			for (uint8_t y = 0; y < 8; y++) {
				for (uint8_t x = 0; x < 8; x++) {
					// La fameuse somme
					somme += ((double)(((int16_t) input[(y << 3) + x]) - 128))
					    * cos_cache[i][x] * cos_cache[j][y];
				}
			}

			// On stocke le résultat dans la matrice fréquentielle
			output[(j << 3) + i] = (int16_t) (0.25 * ci * cj * somme);
		}
	}
}
