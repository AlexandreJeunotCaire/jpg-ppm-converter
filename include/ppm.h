#ifndef _PPM_H_
#define _PPM_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * Structure de donnée contenant une image. La structure est publique pour réduire les surcoûts.
 * gray vaut true si l'image est grise.
 * 
 * Si l'image est grise: pixels est constitué de 1 bloc (gris).
 * Si l'image est colorée: pixels est constitué de 3 blocs (rouge, vert, bleu).
 * 
 * Chaque bloc un tableau de taille width * height, indexé par i = y * width + x.
 */
struct ppm {
	bool gray;
	size_t width, height;
	uint8_t *pixels;
};

/**
 * Lecture d'un fichier au format PPM.
 * Seuls les types P5 (gris) et P6 (couleurs) sont acceptés.
 * Seule un profondeur de composante 0-255 est acceptée.
 * Si le fichier ne peut être lu, NULL est renvoyé.
 */
struct ppm *read_ppm_file(char *filename);

#endif				/* _PPM_H_ */
