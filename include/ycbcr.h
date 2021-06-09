#ifndef _YCBCR_H_
#define _YCBCR_H_

#include <stdint.h>
#include <stdlib.h>

/**
 * Conversion RGB vers YCbCr.
 * Travail en place.
 */
void rgb_to_ycbcr(uint8_t *red_y, uint8_t *green_cb, uint8_t *blue_cr, size_t size);

#endif				/* _YCBCR_H_ */
