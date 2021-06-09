#ifndef _ENCODER_H_
#define _ENCODER_H_

/**
 * Lit le fichier PPM, converti et écrit dans le fichier JPEG.
 */
void ppm_to_jpeg(char *ppm_filename, char *jpeg_filename, uint8_t h[], uint8_t v[], bool verbose);

#endif				/* _ENCODER_H_ */
