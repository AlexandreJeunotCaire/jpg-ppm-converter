#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <encoder.h>

#if 1

char *ppm_to_jpg_extension(const char *ppm_filename, bool verbose);	//remplace .extension par .jpg, cool ou bien ? (ex : salut.ppm > salut.jpg)
void conv_hxv(uint8_t * h, uint8_t * v, char *hixvis);	//Met les bons coefficients dans les tableau h et v

int main(int argc, char *argv[])
{
	/* Salut les kidz
	   En entrée :
	   -v ou --verbose pour avoir + d'infos
	   -h ou --help
	   -s h1xv1,h2xv2,h3xv3 ou --sample=h1xv1,h2xv2,h3xv3
	   -o ./chemin/bliblibloup/fichier.jpg ou --outfile=./chemin/bliblibloup/fichier.jpg

	   Variables utiles là dedans :
	   -> ppm_filename : un char* addresse du fichier .ppm qu'on lui a donné à manger
	   -> jpeg_filename : un char* addresse du fichier .jpg qu'il est censé renvoyer à la fin
	   -> h : un uint8_t * contenant h1, h2, h3
	   -> v : un uint8_t * contentant v1, v2, v3
	 */

	int32_t c;
	bool verbose = false;
	size_t arg_length;
	char *jpeg_filename = NULL;
	uint8_t h[] = { 1, 1, 1 };
	uint8_t v[] = { 1, 1, 1 };

	while (1) {
		static struct option long_options[] = {
			{"verbose", no_argument, 0, 'v'},
			{"help", no_argument, 0, 'h'},
			{"outfile", required_argument, 0, 'o'},
			{"sample", required_argument, 0, 's'},
			{0, 0, 0, 0}
		};

		int32_t option_index = 0;

		c = getopt_long(argc, argv, "vho:s:", long_options, &option_index);

		if (c == -1) {	//Détecte la fin des options
			break;
		}

		switch (c) {
		case 0:
			if (long_options[option_index].flag != 0) {
				break;
			}
			printf("Option %s", long_options[option_index].name);
			if (optarg) {
				printf("avec pour argument %s", optarg);
			}
			printf("\n");
			break;

		case 'v':
			verbose = true;
			break;

		case 'h':
			printf("----------------------------------------------\n");
			printf("Alors me revoilà assez vite !\n\n");
			printf("Usage : ./ppm2jpeg <options> <fichier.ppm>\n");
			printf("Les options sont :\n");
			printf(">>> --verbose ou -v pour avoir plus d'infos\n");
			printf(">>> --help ou -h pour avoir un coup de pouce\n");
			printf
			    (">>> --outfile=sortie.jpg ou -o sortie.jpg pour choisir la destination et le nom du fichier d'arrivée\n");
			printf
			    (">>> --sample=h1xv1,h2xv2,h3xv3 pour définir les facteurs d'échantillonnage hxv des trois composantes de couleur\n\n");
			printf
			    ("Ne t'inquiète pas ça va bien s'passer. Bien s'passer ne t'inquiète pas.\n");
			printf("----------------------------------------------\n");

			if (jpeg_filename != NULL)
				free(jpeg_filename);
			return 0;

		case 'o':
			if (jpeg_filename != NULL)
				free(jpeg_filename);

			arg_length = strlen(optarg);
			jpeg_filename = malloc((arg_length + 1) * sizeof(char));
			strncpy(jpeg_filename, optarg, arg_length + 1);
			break;

		case 's':
			conv_hxv(h, v, optarg);
			break;

		case '?':	//getopt affiche ce qui ne lui plaît pas
			break;

		default:
			abort();
		}

	}

	uint16_t non_option = argc - optind;
	if (non_option != 1) {
		fprintf
		    (stderr, "Tu veux convertir quoi au juste ? Tu ne mets même pas de fichier. Ca ne me fait pas rire du tout. Pas rire du tout.\n");

		if (jpeg_filename != NULL)
			free(jpeg_filename);
		return 0;
	}

	char *ppm_filename = argv[optind++];

	if (jpeg_filename == NULL) {
		jpeg_filename = ppm_to_jpg_extension(ppm_filename, verbose);
	}

	if (verbose) {
		printf("Affichage en mode verbose\n");
		printf("Le nom du fichier d'entrée est %s\n", ppm_filename);
		printf("Le nom du fichier de sortie est %s\n", jpeg_filename);
		printf("On a h1xv1=%dx%d, h2xv2=%dx%d, h3xv3=%dx%d\n", h[0], v[0], h[1], v[1], h[2],
		       v[2]);
	}

	ppm_to_jpeg(ppm_filename, jpeg_filename, h, v, verbose);

	free(jpeg_filename);
	return 0;
}

/**
 * Change l'extension du nom de fichier vers .jpg
 */
char *ppm_to_jpg_extension(const char *ppm_filename, bool verbose) {
	char *jpeg_filename;

	char *last_point = strrchr(ppm_filename, '.');
	if (last_point) {
		char *extension = last_point + 1;
		if (strcmp(extension, "pgm") == 0 || strcmp(extension, "ppm") == 0) {
			if (verbose) {
				printf("Niveau nom, le fichier semble conforme. Il s'agit d'un %s.\n", extension);
			}

			size_t point_index = last_point - ppm_filename;
			jpeg_filename = malloc((point_index + 5) * sizeof(char));

			// Copie le nom jusqu'au dernier point
			*last_point = '\0';
			strcpy(jpeg_filename, ppm_filename);
			*last_point = '.';

			// Copie la fin
			strcpy(jpeg_filename + point_index, ".jpg");	
			
			return jpeg_filename;
		}
	}

	if (verbose) {
		printf("Attention. Le fichier en entrée ne se termine pas par .ppm ou .pgm. Des erreurs peuvent se produire.\n");
	}

	size_t length = strlen(ppm_filename);

	jpeg_filename = malloc((length + 5) * sizeof(char));
	strcpy(jpeg_filename, ppm_filename);
	strcpy(jpeg_filename + length, ".jpg");

	return jpeg_filename;
}

bool verif_criteres_hxv(char *hixvis) {
	size_t longueur = strlen(hixvis);
	if (longueur != 11) {
		fprintf(stderr, "Erreur d'argument sur le sample (longueur). Utilisation : --sample=h1xv1,h2xv2,h3xv3 où les hi et vi sont des entiers compris entre 1 et 4.\n");
		return false;

	}

	if (hixvis[1] != 'x' || hixvis[5] != 'x' || hixvis[9] != 'x') {
		fprintf(stderr, "Erreur d'argument sur le sample. Pensez à séparer les hi et les vi par des 'x'\n");
		return false;
	}

	if (hixvis[3] != ',' || hixvis[7] != ',') {
		fprintf(stderr, "Erreur d'argument sur le sample. Pensez à séparer les hixvi par le caractère ','\n");
		return false;
	}

	for (size_t i = 0; i < longueur; i+=2) {
		if (!(hixvis[i] == '1' || hixvis[i] == '2' || hixvis[i] == '3' || hixvis[i] == '4')) {
			fprintf(stderr, "Erreur d'argument sur le sample. Les hi et vi doivent être des entiers compris entre 1 et 4 inclus.\n");
			return false;
		}
		
	}

	return true;	

}

void conv_hxv(uint8_t * h, uint8_t * v, char *hixvis) {

	bool parametres_avances = true;

	if (verif_criteres_hxv(hixvis)) {
		
		uint8_t result = 0;

		const char *separateurs = ",x";
		size_t indiceH = 1, indiceV = 0;

		char *hixvi = strtok(hixvis, separateurs);
		h[0] = (uint8_t) atoi(hixvi);

		for (size_t i = 1; i < 5; i++) {
			hixvi = strtok(NULL, separateurs);

			if (i % 2 == 1) {
				v[indiceV] = (uint8_t) atoi(hixvi);
				indiceV++;
			}

			else {
				h[indiceH] = (uint8_t) atoi(hixvi);
				indiceH++;
			}
		}

		for (size_t i = 1; i < 3; i++)
		{
			result += h[i] * v[i];
        	if(h[0] % h[i] != 0) {
            	fprintf(stderr, "Erreur au sous-échantillonnage : les paramètres de chrominance ne sont pas des diviseurs par ceux de la luminance horizontalement\n");
           		parametres_avances = false;
				break;

        	}

			if(v[0] % v[i] != 0) {
            	fprintf(stderr, "Erreur au sous-échantillonnage : les paramètres de chrominance ne sont pas des diviseurs par ceux de la luminance verticalement\n");
            	parametres_avances = false;
				break;
        	}
		}

		if (result > 10) {
			fprintf(stderr, "Erreur au sous-échantillonnage : la somme des paramètres est supérieure à 10\n");
			parametres_avances = false;
		}

		if (!parametres_avances) {
			fprintf(stderr, "Erreur sur les paramètres avancés de luminance et chrominance. Pas de sous-échantillonnage.\n");
			for (uint8_t c = 0; c < 3; c++) {
				h[c] = 1;
				v[c] = 1;
			}
		}
		
		
	}

	else {
		printf("Paramètres par défaut conservés : pas de sous-échantillonage.\n");
	}
}

#endif
