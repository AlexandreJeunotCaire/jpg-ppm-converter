#include <stdint.h>
#include <stdlib.h>

#include "block.h"

//Structure qui représente le tableau sous la forme d'un ensemble de mcus.
struct mcus {
	//Nombre de mcu en hauteur et en largeur
	size_t line_nb, col_nb;
	uint8_t *array;
};

struct mcus *cut_in_mcus(struct blocks *blocks, uint8_t h, uint8_t v);

//Fonction qui réalise le sous-échantillonnage. On donne en paramètre les critères de sous-échantillonnage et le tableau de mcu (en RGB). On renvoie le tableau en YCbCr sous-échantillonné
struct mcus *downsampling(struct blocks *y, struct blocks *cb, struct blocks *cr,
				   uint8_t h[], uint8_t v[]);
