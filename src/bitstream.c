#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitstream.h"

struct bitstream {
    FILE *file; // Flux d'écriture
    uint8_t buffer; // Prochain octet
    uint8_t remaining; // bits libres
};

/* Retourne un nouveau bitstream prêt à écrire dans le fichier filename. */
struct bitstream *bitstream_create(const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
		fprintf(stderr, "Impossible d'ouvrir le fichier %s.\n", filename);
		return NULL;
	}

    struct bitstream *stream = malloc(sizeof(struct bitstream));
    stream->file = file;
    stream->buffer = 0;
    stream->remaining = 8;
    return stream;
}


/**
 * Ecrit les 8 bits présent dans le tampon.
 * Gère le byte stuffing.
 */
void bitstream_write_buffer(struct bitstream *stream, bool marker) {
    fwrite(&(stream->buffer), sizeof(uint8_t), 1, stream->file);
    if (!marker && stream->buffer == 0xff) { // Byte stuffing
        uint8_t zero = 0;
        fwrite(&zero, sizeof(uint8_t), 1, stream->file);
    }
}

/*
    Ecrit size bits dans le bitstream. La valeur portée par cet ensemble de
    bits est value. Le paramètre marker permet d'indiquer qu'on est en train
    d'écrire un marqueur de section dans l'entête JPEG ou non (voir section
    2.10.4 du sujet).
*/
void bitstream_write_bits(struct bitstream *stream, uint32_t value, uint8_t size, bool marker) {
    if (size == 0) // Rien à écrire
        return;
    
    value &= (1 << size) - 1; // Efface les bits excédants à gauche de la valeur

    // Début non aligné
    if (size >= stream->remaining) {
        size -= stream->remaining;

        stream->buffer <<= stream->remaining; // Les bits précédants du tampon sont poussés à gauche
        stream->buffer |= value >> size; // Colle les bits les plus à gauche de la valeur, à droite du tampon

        bitstream_write_buffer(stream, marker);

        value &= (1 << size) - 1; // Efface les bits excédants à gauche de la valeur
        stream->buffer = 0; // Vide le tampon
        stream->remaining = 8;
    }

    // Milieu aligné
    while (size >= 8) {
        size -= 8;

        stream->buffer = value >> size; // Met les 8 bits les plus à gauche de la valeur, dans le tampon

        bitstream_write_buffer(stream, marker); 

        value &= (1 << size) - 1; // Efface les bits excédants à gauche de la valeur
        stream->buffer = 0; // Vide le tampon
    }

    // Fin non aligné
    if (size != 0) {
        stream->buffer <<= size; // Les bits précédants du tampon sont poussés à gauche
        stream->buffer |= value; // Colle les bits de la valeur, à droite du tampon

        stream->remaining -= size; // Le tapon se charge

        if (stream->remaining == 0) { // Tampon plein
            bitstream_write_buffer(stream, marker);

            stream->buffer = 0; // Vide le tampon
            stream->remaining = 8;
        }
    }
}

/*
    Force l'exécution des écritures en attente sur le bitstream, s'il en
    existe.
*/
void bitstream_flush(struct bitstream *stream) {
    if (stream->remaining != 8) { // Il reste des données dans le tampon
        stream->buffer <<= stream->remaining; // Les bits précédants du tampon sont poussés à gauche
        bitstream_write_buffer(stream, false);

        stream->buffer = 0; // Vide le tampon
        stream->remaining = 8;
    }

    fflush(stream->file);
}

/*
    Détruit le bitstream passé en paramètre, en libérant la mémoire qui lui est
    associée.
*/
void bitstream_destroy(struct bitstream *stream) {
    bitstream_flush(stream);

    fclose(stream->file);
    free(stream);
}