#ifndef _ZIGZAG_H_
#define _ZIGZAG_H_

#include <stdint.h>

/**
 * Opération de zigzag.
 * 
 * input: matrice 8x8 en entrée.
 * output: vecteur 64 en sortie. 
 */
void zigzag(int16_t * input, int16_t * output);

#endif				/* _ZIGZAG_H_ */
