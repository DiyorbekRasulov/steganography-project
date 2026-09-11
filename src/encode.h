#ifndef ENCODE_H
#define ENCODE_H

#include <stddef.h>
#include <stdint.h>

#include "image.h"
#include "status.h"

/*
 * encode.h
 *
 * the actual steganography, least significant bit embedding
 *
 * every colour value in the image is one byte, so changing the
 * bottom bit moves that colour by one step out of two hundred
 * and fifty six, which the eye cannot pick up
 *
 * original byte
 * 10110100
 *
 * hide a 1 in it
 * 10110101
 *
 * only the last bit moved so the picture looks identical
 */

// writes every bit of blob into the bottom bits of the image
StegStatus encode_data(Image *image, const uint8_t *blob, size_t blob_size);

#endif