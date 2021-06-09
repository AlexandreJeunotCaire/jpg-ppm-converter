#include <stdint.h>

#include "quantification.h"

/**
 * Division terme à terme des matrices.
 * Travail en place.
 */
void divide_matrix(int16_t * numerator, uint8_t * denominator)
{
	for (uint8_t i = 0; i < 64; i++)
		numerator[i] /= denominator[i];
}
