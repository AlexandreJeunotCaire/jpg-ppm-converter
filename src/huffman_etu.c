#if 0

#include <stdlib.h>
#include <stdint.h>

#include "huffman.h"

/* Type opaque représentant un arbre de Huffman. */
struct huff_table {

    uint8_t *nb_symb_per_lengths;
    uint8_t *symbols;
    uint8_t nb_symbols;

};

/*
    Construit un arbre de Huffman à partir d'une table
    de symboles comme présenté en section 2.10.1 du sujet.
    nb_symb_per_lengths est un tableau contenant le nombre
    de symboles pour chaque longueur de 1 à 16,
    symbols est le tableau  des symboles ordonnés,
    et nb_symbols représente la taille du tableau symbols.
*/
extern struct huff_table *huffman_table_build(uint8_t * nb_symb_per_lengths,
					      uint8_t * symbols, uint8_t nb_symbols) {

    struct huff_table *table_huffman = malloc(sizeof(struct huff_table));
    table_huffman->nb_symb_per_lengths = nb_symb_per_lengths;
    table_huffman->symbols = symbols;
    table_huffman->nb_symbols = nb_symbols;

    return table_huffman;
}

/*
    Retourne le chemin dans l'arbre ht permettant d'atteindre
    la feuille de valeur value. nb_bits est un paramètre de sortie
    permettant de stocker la longueur du chemin retourné.
*/
extern uint32_t huffman_table_get_path(struct huff_table *ht, uint8_t value, uint8_t * nb_bits) {
    uint8_t indice_symbole = 0;
    uint32_t resultat = 0;

    for (uint8_t i = 0; i < ht->nb_symbols; i++) {
        if (ht->symbols[i] == value) {
           indice_symbole = i;
           break;
        }
        
    }

    return resultat;

    //TODO (et ça ne sera JAMAIS fait car il est 18h45 le jour du rendu, ha!)
    
}

/*
   Retourne le tableau des symboles associé à l'arbre de
   Huffman passé en paramètre.
*/
extern uint8_t *huffman_table_get_symbols(struct huff_table *ht) {
    return ht->symbols;
}

/*
    Retourne le tableau du nombre de symboles de chaque longueur
    associé à l'arbre de Huffman passé en paramètre.
*/
extern uint8_t *huffman_table_get_length_vector(struct huff_table *ht) {
    return ht->nb_symb_per_lengths;
}

/*
    Détruit l'arbre de Huffman passé en paramètre et libère
    toute la mémoire qui lui est associée.
*/

extern void huffman_table_destroy(struct huff_table *ht) {
    free(ht);
    //TODO
}

#endif
