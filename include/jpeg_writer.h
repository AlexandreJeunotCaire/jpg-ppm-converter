#ifndef _JPEG_WRITER_H_
#define _JPEG_WRITER_H_

#include <stdint.h>

#include <bitstream.h>
#include <huffman.h>

enum color_component {
	Y,
	Cb,
	Cr,
	COLOR_COMPONENTS
};

enum sample_type {
	DC,
	AC,
	SAMPLE_TYPES
};

enum direction {
	H,
	V,
	DIRECTIONS
};

/**
 * Ecrit les sections SOI, APP0 et COM.
 */
void jpeg_write_SOI_APP0_COM(struct bitstream *stream);

/**
 * Ecrit la section DQT.
 * 
 * tables: Tables de quantification indéxées par composante.
 */
void jpeg_write_DQT(struct bitstream *stream, uint8_t * tables[], uint8_t components);

/**
 * Ecrit la section SOF0.
 * 
 * h et v: Facteurs de sous-échantillonage indexés par composante.
 */
void jpeg_write_SOF0(struct bitstream *stream, uint16_t width, uint16_t height, uint8_t components,
			uint8_t h[], uint8_t v[]);

/**
 * Ecrit la section DHT.
 */
void jpeg_write_DHT(struct bitstream *stream, struct huff_table *table, enum sample_type type,
		    enum color_component component);

/**
 * Ecrit la section SOS. 
 */
void jpeg_write_SOS(struct bitstream *stream, uint8_t components);

/**
 * Ecrit la section EOI.
 */
void jpeg_write_EOI(struct bitstream *stream);

#endif				/* _JPEG_WRITER_H_ */
