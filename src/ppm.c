#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "ppm.h"

/**
 * Lit une ligne de texte du fichier, ignore les commentaires et les lignes vides.
 * Si la ligne ne peut être lue, la tampon sera une chaîne vide.
 */
void read_line(char *buffer, size_t buffer_len, FILE * file)
{
	// Recommence tant que nous n'avons pas récupéré une ligne valide et que le fichier n'est pas terminé.
	while (fgets(buffer, buffer_len, file)) {
		// Coupe la chaîne avant le commentaire, s'il existe.
		char *comment = strchr(buffer, '#');
		if (comment != NULL)
			*comment = '\0';

		// Coupe les espaces inutiles en fin de chaîne.
		char *end = buffer + strlen(buffer) - 1;
		while (end > buffer && isspace(*end))
			end--;
		*(end + 1) = '\0';

		// Vérifie que le restant de la chaîne n'est pas vide.
		if (end + 1 != buffer)
			return;
	}

	// Impossible de récupérer une ligne valide.
	*buffer = '\0';
}

struct ppm *read_ppm(FILE * file)
{
	// Lecture des entêtes.
	char buffer[71];	// Aucune ligne ne dépasse 70 caractères selon la spec.

	// Type de l'image
	read_line(buffer, sizeof(buffer), file);
	bool gray = strcmp(buffer, "P5") == 0;
	if (!gray && strcmp(buffer, "P6") != 0) {
		fprintf(stderr, "Le type %s n'est pas accepté.\n", buffer);
		return NULL;
	}
	// Dimensions de l'image
	read_line(buffer, sizeof(buffer), file);
	char *sep = strchr(buffer, ' ');
	if (sep == NULL) {
		fprintf(stderr, "Erreur : les dimensions %s sont invalides.\n", buffer);
		return NULL;
	}

	*sep = '\0';
	int width = atoi(buffer), height = atoi(sep + 1);
	if (width <= 0 || height <= 0) {
		*sep = ' ';
		fprintf(stderr, "Erreur : les dimensions %s sont invalides.\n", buffer);
		return NULL;
	}
	// Profondeur de composante
	read_line(buffer, sizeof(buffer), file);
	if (strcmp(buffer, "255") != 0) {
		fprintf(stderr, "La profondeur de composante %s n'est pas acceptée.\n", buffer);
		return NULL;
	}
	// Lecture des pixels
	size_t size = width * height;
	uint8_t *pixels;
	size_t read = 0;

	if (gray) {
		pixels = calloc(size, sizeof(uint8_t));
		read = fread(pixels, 1, size, file);
	} else {
		pixels = calloc(size * 3, sizeof(uint8_t));
		// Découpage en 3 blocs.
		uint8_t *red = pixels;
		uint8_t *green = red + size * sizeof(uint8_t);
		uint8_t *blue = green + size * sizeof(uint8_t);

		// Lecture pixel par pixel.
		uint8_t rgb[3];
		for (size_t i = 0; i < size; i++) {
			if (fread(rgb, 3, 1, file) == 0)	// Fin de flux
				break;

			red[i] = rgb[0];
			green[i] = rgb[1];
			blue[i] = rgb[2];
			read++;
		}
	}

	if (read != size) {
		fprintf(stderr, "Nombre de pixels invalides. lus: %lu attendus: %lu", read, size);
		free(pixels);
		return NULL;
	}
	// Emballe et livre le colis.
	struct ppm *image = malloc(sizeof(struct ppm));
	image->gray = gray;
	image->width = width;
	image->height = height;
	image->pixels = pixels;
	return image;
}

/**
 * Lecture d'un fichier au format PPM.
 * Seuls les types P5 (gris) et P6 (couleurs) sont acceptés.
 * Seule un profondeur de composante 0-255 est acceptée.
 * Si le fichier ne peut être lu, NULL est renvoyé.
 */
struct ppm *read_ppm_file(char *filename)
{
	// Ouvre le fichier en lecture binaire.
	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
		fprintf(stderr, "Impossible d'ouvrir le fichier %s.\n", filename);
		return NULL;
	}

	struct ppm *image = read_ppm(file);

	if (fclose(file)) {
		fprintf(stderr, "Erreur lors de la fermeture du fichier.\n");
	}

	return image;
}
