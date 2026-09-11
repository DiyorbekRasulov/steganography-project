#ifndef DECODE_H
#define DECODE_H

#include <stddef.h>
#include <stdint.h>

#include "image.h"
#include "status.h"

/*
 * decode.h
 *
 * pulls the hidden bits back out and rebuilds them into bytes
 *
 * this is the encoder run backwards, eight carrier bytes at a
 * time turning back into one payload byte
 */

/*
 * reads count bytes starting at byte number start
 *
 * start is counted in rebuilt payload bytes, not carrier bytes,
 * so asking for start of 22 gives you whatever follows the header
 */
StegStatus decode_bytes(const Image *image,
                        size_t start,
                        size_t count,
                        uint8_t *out);

#endif