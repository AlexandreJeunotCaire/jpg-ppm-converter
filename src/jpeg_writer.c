#include <stdint.h>
#include <stdbool.h>

#include <jpeg_writer.h>
#include <bitstream.h>
#include <huffman.h>

/**
 * Ecrit les marqueurs SOI, APPx et COM.
 */
void jpeg_write_SOI_APP0_COM(struct bitstream *stream)
{
	// SOI
	bitstream_write_bits(stream, 0xffd8, 16, true);	// Marqueur
	// APP0
	bitstream_write_bits(stream, 0xffe0, 16, true);	// Marqueur
	bitstream_write_bits(stream, 16, 16, false);	// Longueur
	bitstream_write_bits(stream, 'J', 8, false);	// Format
	bitstream_write_bits(stream, 'F', 8, false);
	bitstream_write_bits(stream, 'I', 8, false);
	bitstream_write_bits(stream, 'F', 8, false);
	bitstream_write_bits(stream, '\0', 8, false);
	bitstream_write_bits(stream, 1, 8, false);	// Version majeure
	bitstream_write_bits(stream, 1, 8, false);	// Version mineure
	for (uint8_t i = 0; i < 7; i++) {
		bitstream_write_bits(stream, 0, 8, false);
	}
	// COM
	bitstream_write_bits(stream, 0xfffe, 16, true);	// Marqueur
	bitstream_write_bits(stream, 2, 16, false);	// Longueur
	// Pas de commentaire
}

/**
 * Ecrit le marqueur DQT.
 * 
 * tables: Tables de quantification indéxées par composante.
 */
void jpeg_write_DQT(struct bitstream *stream, uint8_t * tables[], uint8_t components)
{
	bitstream_write_bits(stream, 0xffdb, 16, true);	// Marqueur
	bitstream_write_bits(stream, 2 + components * 65, 16, false);	// Longueur

	for (uint8_t c = 0; c < components; c++) {
		bitstream_write_bits(stream, 0, 4, false);	// Précision
		bitstream_write_bits(stream, c, 4, false);	// Indice
		for (uint8_t i = 0; i < 64; i++) {	// Valeurs de la table
			bitstream_write_bits(stream, tables[c][i], 8, false);
		}
	}
}

/**
 * Ecrit le marqueur SOF0.
 * 
 * h et v: Facteurs de sous-échantillonage indéxés par composante.
 */
void jpeg_write_SOF0(struct bitstream *stream, uint16_t width, uint16_t height, uint8_t components,
		    uint8_t h[], uint8_t v[])
{
	bitstream_write_bits(stream, 0xffc0, 16, true);	// Marqueur
	bitstream_write_bits(stream, 3 * components + 8, 16, false);	// Longueur
	bitstream_write_bits(stream, 8, 8, false);	// Précision par composante
	bitstream_write_bits(stream, height, 16, false);	// Hauteur
	bitstream_write_bits(stream, width, 16, false);	// Largeur
	bitstream_write_bits(stream, components, 8, false);	// Nombre de composantes

	for (uint8_t c = 0; c < components; c++) {
		bitstream_write_bits(stream, c, 8, false);	// Identifiant de composante
		bitstream_write_bits(stream, h[c], 4, false);	// Facteur de sous-échantillonage horizontal
		bitstream_write_bits(stream, v[c], 4, false);	// Facteur de sous-échantillonage vertical
		bitstream_write_bits(stream, c, 8, false);	// Indice Quantification
	}
}

/**
 * Ecrit un marqueur DHT.
 */
void jpeg_write_DHT(struct bitstream *stream, struct huff_table *table, enum sample_type type,
		    enum color_component component)
{
	// Calcule le nombre de symboles
	uint8_t *length_vector = huffman_table_get_length_vector(table);
	uint8_t total_count = 0;
	for (uint8_t i = 0; i < 16; i++) {
		total_count += length_vector[i];
	}

	bitstream_write_bits(stream, 0xffc4, 16, true);	// Marqueur
	bitstream_write_bits(stream, total_count + 19, 16, false);	// Longueur
	bitstream_write_bits(stream, 0, 3, false);
	bitstream_write_bits(stream, type, 1, false);	// Type DC ou AC
	bitstream_write_bits(stream, component, 4, false);	// Indice

	for (uint8_t i = 0; i < 16; i++) {
		bitstream_write_bits(stream, length_vector[i], 8, false);	// Nombre de symboles de longueur (i+1)
	}

	uint8_t *symbols = huffman_table_get_symbols(table);
	for (uint8_t i = 0; i < total_count; i++) {	// Tout les symboles
		bitstream_write_bits(stream, symbols[i], 8, false);
	}
}

/**
 * Ecrit le marqueur SOS.
 */
void jpeg_write_SOS(struct bitstream *stream, uint8_t components)
{
	bitstream_write_bits(stream, 0xffda, 16, true);	// Marqueur
	bitstream_write_bits(stream, 2 * components + 6, 16, false);	// Longueur
	bitstream_write_bits(stream, components, 8, false);	// Nombre de composantes

	for (uint8_t c = 0; c < components; c++) {
		bitstream_write_bits(stream, c, 8, false);	// Identifiant de composante
		bitstream_write_bits(stream, c, 4, false);	// Indice Huffman DC
		bitstream_write_bits(stream, c, 4, false);	// Indice Huffman AC
	}

	bitstream_write_bits(stream, 0, 8, false);	// Premier indice de la sélection spéctrale
	bitstream_write_bits(stream, 63, 8, false);	// Dernier indice de la sélection spéctrale
	bitstream_write_bits(stream, 0, 8, false);	// Approximation successive
}

/**
 * Ecrit le marqueur EOI.
 */
void jpeg_write_EOI(struct bitstream *stream)
{
	bitstream_flush(stream);
	bitstream_write_bits(stream, 0xffd9, 16, true);	// Marqueur
}
