#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "matrix.h"

/**
 * Affiche la matrice de dimensions 8x8.
 */
void print_block_uint8(uint8_t * matrix)
{
	print_matrix_uint8(matrix, 8, 8);
}

/**
 * Affiche le vecteur de dimension donnée.
 */
void print_vector_uint8(uint8_t * vector, size_t length)
{
	print_matrix_uint8(vector, length, 1);
}

/**
 * Affiche la matrice de dimensions données.
 */
void print_matrix_uint8(uint8_t * matrix, size_t width, size_t height)
{
	size_t index = 0;
	for (size_t j = 0; j < height; j++) {
		for (size_t i = 0; i < width; i++) {
			printf("%02" PRIx8 " ", matrix[index++]);
		}
		printf("\n");
	}
}

/**
 * Affiche la matrice de dimensions 8x8.
 */
void print_block_int16(int16_t * matrix)
{
	print_matrix_int16(matrix, 8, 8);
}

/**
 * Affiche le vecteur de dimension donnée.
 */
void print_vector_int16(int16_t * vector, size_t length)
{
	print_matrix_int16(vector, length, 1);
}

/**
 * Affiche la matrice de flottants de dimensions données.
 */
void print_matrix_int16(int16_t * matrix, size_t width, size_t height)
{
	size_t index = 0;
	for (size_t j = 0; j < height; j++) {
		for (size_t i = 0; i < width; i++) {
			printf("%04" PRIx16 " ", (uint16_t) matrix[index++]);
		}
		printf("\n");
	}
}
