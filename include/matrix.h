#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <stdint.h>
#include <stdlib.h>

/**
 * Affiche la matrice de dimensions 8x8.
 */
void print_block_uint8(uint8_t * matrix);

/**
 * Affiche le vecteur de dimension donnée.
 */
void print_vector_uint8(uint8_t * vector, size_t length);

/**
 * Affiche la matrice de dimensions données.
 */
void print_matrix_uint8(uint8_t * matrix, size_t width, size_t height);

/**
 * Affiche la matrice de dimensions 8x8.
 */
void print_block_int16(int16_t * matrix);

/**
 * Affiche le vecteur de dimension donnée.
 */
void print_vector_int16(int16_t * vector, size_t length);

/**
 * Affiche la matrice de dimensions données.
 */
void print_matrix_int16(int16_t * matrix, size_t width, size_t height);

#endif				/* _MATRIX_H_ */
