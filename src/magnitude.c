#include <stdint.h>
#include <stdbool.h>

#include "magnitude.h"

/**
 * Code une valeur en une magnitude et un index.
 */
void to_magnitude_index(int16_t value, uint8_t * magnitude, uint16_t * index)
{
	// Valeur absolue
	uint16_t abs_value = value < 0 ? -value : value;

	// Calcule la magnitude
	*magnitude = 0;
	uint16_t temp = abs_value;
	while (temp) {
		(*magnitude)++;
		temp >>= 1;
	}

	// Calcule l'index
	if (value < 0)
		*index = ~abs_value & ((1 << *magnitude) - 1);	// Complémente et restreint la longueur
	else
		*index = abs_value;
}

/**
 * Décode une magnitude et un index en une valeur.
 */
int16_t from_magnitude_index(uint8_t magnitude, uint16_t index)
{
	if ((index >> (magnitude - 1)) == 0) {	// Le bit le plus à gauche donne le signe
		// Négatif
		return -(~index & ((1 << magnitude) - 1));	// Complémente et restreint la longueur, puis change de signe
	}
	// Positif
	return index;
}
