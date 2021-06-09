#ifndef _MAGNITUDE_H_
#define _MAGNITUDE_H_

#include <stdint.h>

/**
 * Code une valeur une magnitude et un index.
 */
void to_magnitude_index(int16_t value, uint8_t * magnitude, uint16_t * index);

/**
 * Décode une magnitude et un index en une valeur.
 */
int16_t from_magnitude_index(uint8_t magnitude, uint16_t index);

#endif				/* _MAGNITUDE_H_ */
