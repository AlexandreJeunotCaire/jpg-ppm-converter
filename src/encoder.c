#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "encoder.h"
#include "jpeg_writer.h"
#include "bitstream.h"
#include "huffman.h"

#include "qtables.h"
#include "htables.h"

#include "ppm.h"
#include "ycbcr.h"
#include "dct.h"
#include "zigzag.h"
#include "quantification.h"
#include "magnitude.h"
#include "matrix.h"
#include "block.h"
#include "downsampling.h"

/**
 * Lit le fichier PPM, converti et écrit dans le fichier JPEG.
 */
void ppm_to_jpeg(char *ppm_filename, char *jpeg_filename, uint8_t h[], uint8_t v[], bool verbose)
{
	printf("Conversion du fichier %s vers le fichier %s ...\n",
	       ppm_filename, jpeg_filename);

	struct ppm *image = read_ppm_file(ppm_filename);
	if (image == NULL) {
		fprintf(stderr, "Echec de la conversion.\n");
		return;
	}

	uint8_t components = image->gray ? 1 : 3;

	if (verbose) {
		printf("Nombre de composantes: %u\n", components);
	}

	bool do_downsampling = false;
	for (uint8_t c = 0; c < components; c++) {
		if (h[c] != 1 || v[c] != 1) {
			do_downsampling = true;
			break;
		}
	}

	if (do_downsampling && image->gray) {
		printf("Le sous-échantillonage n'est pas disponible pour une image grise.\n");
		free(image->pixels);
		free(image);
		return;
	}

	struct bitstream *stream = bitstream_create(jpeg_filename);
	if (stream == NULL) {
		fprintf(stderr, "Echec de la conversion.\n");
		free(image->pixels);
		free(image);
		return;
	}

	if (verbose) {
		printf("Ecriture de l'en-tête ...\n");
		printf("SOI APP0 COM\n");
	}
	// Format
	jpeg_write_SOI_APP0_COM(stream);

	if (verbose) {
		printf("DQT\n");
	}
	// Tables de quantifications
	jpeg_write_DQT(stream, quantification_tables, components);

	if (verbose) {
		printf("SOF0\n");
	}
	// Informations sur l'image
	jpeg_write_SOF0(stream, image->width, image->height, components, h, v);

	// Tables de Huffman
	struct huff_table *htables[SAMPLE_TYPES][components];
	for (uint8_t type = 0; type < SAMPLE_TYPES; type++) {
		for (uint8_t c = 0; c < components; c++) {
			// Construit la table à partir des données
			htables[type][c] =
			    huffman_table_build(htables_nb_symb_per_lengths[type][c],
						htables_symbols[type][c],
						htables_nb_symbols[type][c]);

			// Ecrit la table
			if (verbose) {
				printf("DHT\n");
			}
			jpeg_write_DHT(stream, htables[type][c], type, c);
		}
	}

	if (verbose) {
		printf("SOS\n");
	}
	// Début des données brutes
	jpeg_write_SOS(stream, components);

	if (verbose) {
		printf("Découpage des blocs ...\n");
	}

	// Découpage des blocs (RGB)
	size_t size = image->width * image->height * sizeof(uint8_t);
	struct blocks *blocks[components];
	for (uint8_t c = 0; c < components; c++) {
		blocks[c] = cut_blocks(image->pixels + c * size, image->width, image->height);
	}

	// Conversion RGB -> YCbCr, en place
	if (!image->gray) {
		if (verbose) {
			printf("Conversion RGB vers YCbCr ...\n");
		}
		rgb_to_ycbcr(blocks[0]->array, blocks[1]->array, blocks[2]->array, blocks[0]->width * blocks[0]->height * 64);
	}

	// Néttoyage
	free(image->pixels);
	free(image);

	// MCU
	size_t mcu_width, mcu_height;
	uint8_t *mcu_array; // Optionnel, existe pour pouvoir néttoyer
	uint8_t *raw_data[components];

	if (do_downsampling) {
		if (verbose) {
			printf("Sous-échantillonage ...\n");
		}

		struct mcus *mcus = downsampling(blocks[0], blocks[1], blocks[2], h, v);
		mcu_width = mcus->line_nb;
		mcu_height = mcus->col_nb;
		mcu_array = mcus->array;

		free(mcus);

		size_t mcus_component_offset = 0;
		for (uint8_t c = 0; c < components; c++) {
			raw_data[c] = mcu_array + mcus_component_offset;
			mcus_component_offset = h[c] * v[c] * mcu_width * mcu_height * 64 * sizeof(uint8_t);

			// Néttoyage
			free(blocks[c]->array);
			free(blocks[c]);
		}
	} else {
		mcu_width = blocks[0]->width;
		mcu_height = blocks[0]->height;

		for (uint8_t c = 0; c < components; c++) {
			raw_data[c] = blocks[c]->array;

			// Néttoyage
			free(blocks[c]);
		}
	}

	if (verbose) {
		printf("Conversion et écriture des données brutes ...\n");
	}

	// Préparation DPCM
	int16_t previous_dc[components];
	for (uint8_t c = 0; c < components; c++)
		previous_dc[c] = 0;

	// Quelques champs utiles
	uint8_t magnitude;
	uint16_t index;
	uint32_t hf;
	uint8_t bits;

	size_t block_offset[components];
	for (uint8_t c = 0; c < components; c++) {
		block_offset[c] = 0;
	}

	// Pour chaque mcu
	for (size_t mcu_y = 0; mcu_y < mcu_height; mcu_y++) {
		for (size_t mcu_x = 0; mcu_x < mcu_width; mcu_x++) {
			if (verbose) {
				printf("\nMCU %lu, %lu:\n", mcu_x, mcu_y);
			}

			// Pour chaque composante
			for (uint8_t c = 0; c < components; c++) {
				if (verbose) {
					printf("\nComposante %u:\n", c);
				}

				// Pour chaque bloc
				for (uint8_t block_y = 0; block_y < h[c]; block_y++) {
					for (uint8_t block_x = 0; block_x < v[c]; block_x++) {
						if (verbose) {
							printf("\nBloc %u, %u:\n", block_x, block_y);
						}

						// Données brutes (Y ou Cb ou Cr)
						uint8_t *block = raw_data[c] + block_offset[c];
						if (verbose) {
							print_block_uint8(block);
						}
						// DCT
						int16_t block_freq[64];
						dct(block, block_freq);
						if (verbose) {
							printf("\nDCT:\n");
							print_block_int16(block_freq);
						}
						// Zig-zag
						int16_t block_zz[64];
						zigzag(block_freq, block_zz);
						if (verbose) {
							printf("\nZig-Zag:\n");
							print_block_int16(block_zz);
						}
						// Quantification
						divide_matrix(block_zz, quantification_tables[c]);
						if (verbose) {
							printf("\nQuantification:\n");
							print_block_int16(block_zz);
						}
						// ECRITURE DC

						// DPCM
						int16_t rel_dc = block_zz[0] - previous_dc[c];
						previous_dc[c] = block_zz[0];

						if (verbose) {
							printf("\nrdc: %i\n", rel_dc);
						}

						// Codage magnitude + indice
						to_magnitude_index(rel_dc, &magnitude, &index);

						// Ecriture Huffman(magnitude)
						hf = huffman_table_get_path(htables[DC][c], magnitude, &bits);
						bitstream_write_bits(stream, hf, bits, false);

						// Ecriture indice
						bitstream_write_bits(stream, index, magnitude, false);

						// ECRITURE AC

						// RLE
						uint8_t zeros = 0;
						for (uint8_t i = 1; i < 64; i++) {
							if (block_zz[i] != 0) {
								while (zeros >= 16) {
									// Ecriture Huffman(paquet de 16 zéros)
									if (verbose) {
										printf("rle: 240\n");
									}

									hf = huffman_table_get_path(htables[AC][c],
												    0xF0, &bits);
									bitstream_write_bits(stream, hf, bits,
											     false);
									zeros -= 16;
								}

								// Codage magnitude + indice
								to_magnitude_index(block_zz[i], &magnitude, &index);

								// Symbole RLE
								uint8_t rle = (zeros << 4) | magnitude;

								if (verbose) {
									printf("rle: %u\n", rle);
								}
								// Ecriture Huffman(rle)
								hf = huffman_table_get_path(htables[AC][c], rle,
											    &bits);
								bitstream_write_bits(stream, hf, bits, false);

								// Ecriture indice
								bitstream_write_bits(stream, index, magnitude,
										     false);

								zeros = 0;
							} else {
								zeros++;
							}
						}

						// EOB
						if (zeros != 0) {
							if (verbose) {
								printf("rle: 0 (EOB)\n");
							}
							hf = huffman_table_get_path(htables[AC][c], 0, &bits);
							bitstream_write_bits(stream, hf, bits, false);
						}

						if (verbose) {
							printf("\n");
						}

						block_offset[c] += 64 * sizeof(uint8_t);
					}
				}
			}
		}
	}

	// Néttoyage
	if (do_downsampling) {
		free(mcu_array);
	} else {
		for (uint8_t c = 0; c < components; c++) {
			free(raw_data[c]);
		}
	}

	for (uint8_t type = 0; type < SAMPLE_TYPES; type++) {
		for (uint8_t c = 0; c < components; c++) {
			huffman_table_destroy(htables[type][c]);
		}
	}

	if (verbose) {
		printf("Ecriture du pied ...");
	}
	// Fin
	jpeg_write_EOI(stream);

	bitstream_destroy(stream);

	printf("\nConversion terminée.\n");
}
