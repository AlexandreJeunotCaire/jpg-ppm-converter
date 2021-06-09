#ifndef _DCT_H_
#define _DCT_H_

#include <stdint.h>

/**
 * Calcul de la transformée en cosinus discrète.
 *
 * input: matrice carrée 8x8 en entrée (spatiale).
 * output: matrice carrée 8x8 en sortie (fréquentielle).
 */
void dct(uint8_t * input, int16_t * output);

#endif				/* _DCT_H_ */
