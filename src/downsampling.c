#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "downsampling.h"

//Ici, h et v sont des tableaux à 3 valeurs sauf indication contraire.


//Fonction qui réorganise les blocks en mcus.
struct mcus *cut_in_mcus(struct blocks *blocks, uint8_t h, uint8_t v) {
    
    //On récupère la taille du MCU (en blocs), en divisant la taille du tableau (en blocs) par le h et v arrondi au supérieur.
    size_t line_nb = blocks->height / v;
    if(blocks->height % v != 0)
        line_nb++;

    size_t col_nb = blocks->width / h;
    if(blocks->width % h != 0)
        col_nb++;
    
    //Allocation
    struct mcus *mcus = malloc(sizeof(struct mcus)); 
    mcus->array = calloc(64 * line_nb * v * col_nb * h, sizeof(uint8_t));
    mcus->line_nb = line_nb;
    mcus->col_nb = col_nb;

    size_t index = 0;
    for(size_t i=0; i < line_nb; i++){
        for(size_t j=0; j < col_nb; j++){
            for(size_t block_y=0; block_y < v; block_y++){
                for(size_t block_x=0; block_x < h; block_x++){
                    size_t tx = i * h + block_x;
                    if(tx >= blocks->height)
                        tx = blocks->height - 1;
                    
                    size_t ty = j * v + block_y;
                    if(ty >= blocks->width)
                        ty = blocks->width - 1;

                    //On copie le bloc
                    memcpy(mcus->array + index, blocks->array + (ty * blocks->width + tx) * sizeof(uint8_t), 64 * sizeof(uint8_t)); 
                    index += 64 * sizeof(uint8_t);
                }
            }
        }
    }  
    return mcus;

}

//Fait les sommes sur un bloc
void moyenne_block(uint8_t block1[], uint8_t block2[], uint8_t diviseur) {
    for (uint8_t i = 0; i < 64; i++) {
        block1[i] = (block1[i] + block2[i]) / diviseur;
    }
}

//On a déjà un découpage en mcu ici. h et v sont à 2 valeurs : la valeur pour y et la valeur pour la chrominance dont on essaye d'avoir le downsampling
uint8_t *mcu_downsampling_chr(struct mcus *chr, uint8_t h[], uint8_t v[]){
    //Initialisation du tableau de retour
    uint8_t *array = calloc(chr->line_nb * chr->col_nb * v[1] * h[1], sizeof(uint8_t));
    
    //Tableau temporaire qui va contenir le résultat de la fusion horizontale
    uint8_t *tmp_h = calloc(h[1] * v[0] * 64, sizeof(uint8_t));

    //Tableau temporaire qui va contenir le résultat de la fusion verticale
    uint8_t *tmp_v = calloc(h[1] * v[1] * 64, sizeof(uint8_t));

    //Variables de travail
    size_t taille_mcu = 64 * h[0] * v[0];
    uint8_t *mcu = calloc(taille_mcu, sizeof(uint8_t));
    

    //Itération sur tous les mcus
    for(size_t j=0; j < chr->col_nb; j++){

        for(size_t i=0; i < chr->line_nb; i++){

            //Initialisation des valeurs de tmp
            memset(tmp_h, 0, h[1] * v[0] * 64);
            memset(tmp_v, 0, h[1] * v[1] * 64);

            //On copie dans le tableau de travail les valeurs du mcu correspondant
            memcpy(mcu, chr->array + taille_mcu * (i * chr->col_nb + j), taille_mcu);

            //On fait d'abord la fusion horizontale
            for(uint8_t h_fusion_index=0; h_fusion_index < h[0] * v[1]; h_fusion_index++){
                

                //On fait la moyenne sur chaque élément horizontal
                for(uint8_t mean_element=0; mean_element < h[0] / h[1]; mean_element++){
                    //On prend v[0] / v[1] elements verticaux
                    for(uint8_t vert_elem_nb=0; vert_elem_nb < v[0] / v[1]; vert_elem_nb++){

                        //On somme sur les blocks
                        moyenne_block(tmp_h + 64 * (h_fusion_index * (v[0] / v[1]) + vert_elem_nb), mcu + 64 * (h_fusion_index *(h[0] / h[1]) + mean_element + vert_elem_nb * h[0]), h[0] / h[1]);
                    }
                }
            }

            //Fusion verticale
            for(uint8_t v_fusion_index=0; v_fusion_index < h[1] * v[1]; v_fusion_index++) {
                        
                //Somme des blocks
                for(uint8_t mean_index = 0; mean_index < v[1] / v[0]; mean_index++){
                    moyenne_block(tmp_v + v_fusion_index * 64, tmp_h + 64 * (v_fusion_index * (v[1] / v[0]) + mean_index), v[0] / v[1]);
                }
            }

            //Ajout dans le tableau final
            memcpy(array + (i * chr->col_nb + j) * h[1] * v[1], tmp_v, h[1] * v[1] * 64);
        }
    }
    
    return array;
}


//Fonction qui réalise le sous-échantillonnage. On donne en paramètre les critères de sous-échantillonnage et le tableau de mcu (en RGB). On renvoie le tableau en YCbCr sous-échantillonné
struct mcus *downsampling(struct blocks *y, struct blocks *cb, struct blocks *cr, uint8_t h[], uint8_t v[]){

    //Réorganisation du tableau en mcus
    struct mcus *mcu_y = cut_in_mcus(y, h[0], v[0]);
    struct mcus *mcu_cb = cut_in_mcus(cb, h[0], v[0]);
    struct mcus *mcu_cr = cut_in_mcus(cr, h[0], v[0]);

    //Comme les trois tableaux sont de même taille, on va avoir le même nombre de mcus verticalement et horizontalement pour ces trois tableaux
    size_t line_nb = mcu_y->line_nb;
    size_t col_nb = mcu_y->col_nb;

    //Allocation 
    struct mcus *mcus = malloc(sizeof(struct mcus));
    mcus->array = calloc(64 * (v[0] * h[0] + v[1] * h[1] + v[2] * h[2]) * mcu_y->line_nb * mcu_y->col_nb, sizeof(uint8_t));
    mcus->line_nb = line_nb;
    mcus->col_nb = col_nb;

    //Copie des valeurs de luminance
    memcpy(mcus->array, mcu_y->array, line_nb * col_nb * v[0] * h[0] * 64 * sizeof(uint8_t));
    
    //Sous-échantillonnage des valeurs de cb et copie des valeurs
    uint8_t h_cb[2] = {h[0], h[1]};
    uint8_t v_cb[2] = {v[0], v[1]};
    uint8_t *array_cb = mcus->array + line_nb * col_nb * v[0] * h[0] * 64 * sizeof(uint8_t);
    memcpy(array_cb, mcu_downsampling_chr(mcu_cb, h_cb, v_cb), line_nb * col_nb * v[1] * h[1] * 64 * sizeof(uint8_t));

    //Sous-échantillonnage de cr et copie des valeurs
    uint8_t h_cr[2] = {h[0], h[2]};
    uint8_t v_cr[2] = {v[0], v[2]};
    uint8_t *array_cr = array_cb + line_nb * col_nb * v[1] * h[1] * 64 * sizeof(uint8_t);
    memcpy(array_cr, mcu_downsampling_chr(mcu_cr, h_cr, v_cr), line_nb * col_nb * v[2] * h[2] * 64 * sizeof(uint8_t));

    free(mcu_y->array);
    free(mcu_y);

    free(mcu_cb->array);
    free(mcu_cb);

    free(mcu_cr->array);
    free(mcu_cr);
    return mcus;
}