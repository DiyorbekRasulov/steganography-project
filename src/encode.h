#ifndef ENCODE_H
#define ENCODE_H

#include <stdint.h>
#include "image.h"

/*
 * encode.h
 *
 * Contains the actual steganography encoding logic.
 *
 * We will probably use:
 * Least Significant Bit (LSB) encoding first.
 *
 * Example:
 * Original byte:
 * 10110100
 *
 * Hide bit '1':
 * 10110101
 *
 * Only the last bit changes,
 * so the image change is visually invisible.
 */

int encode_data(Image *image,
                uint8_t *secret_data,
                int secret_size);

#endif
