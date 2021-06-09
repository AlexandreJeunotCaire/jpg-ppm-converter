#include <stdint.h>
#include <stdlib.h>

#include "ycbcr.h"

/**
 * Conversion RGB vers YCbCr.
 * Travail en place.
 */
void rgb_to_ycbcr(uint8_t *red_y, uint8_t *green_cb, uint8_t *blue_cr, size_t size) {
	for (size_t i = 0; i < size; i++) {
		uint8_t r = red_y[i], g = green_cb[i], b = blue_cr[i];
		red_y[i] = (uint8_t) (0.299 * r + 0.587 * g + 0.114 * b);
		green_cb[i] = (uint8_t) (-0.1687 * r - 0.3313 * g + 0.5 * b + 128);
		blue_cr[i] = (uint8_t) (0.5 * r - 0.4187 * g - 0.0813 * b + 128);
	}
}
