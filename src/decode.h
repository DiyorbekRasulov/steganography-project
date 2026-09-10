#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>
#include "image.h"

/*
 * decode.h
 *
 * Extract hidden data from images.
 *
 * This reverses the encoding process.
 */

uint8_t *decode_data(Image *image, int *output_size);

#endif
