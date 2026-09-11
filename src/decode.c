#include "decode.h"
#include "utils.h"

/*
 * decode.c
 *
 * reading is done in chunks rather than all at once because the
 * header has to be read and checked before we know how much of
 * the rest is real, and we do not want to allocate for a length
 * we have not validated yet
 */

StegStatus decode_bytes(const Image *image,
                        size_t start,
                        size_t count,
                        uint8_t *out)
{
    if (image == NULL || out == NULL)
    {
        return STEG_ERR_USAGE;
    }

    size_t available = image_usable_bytes(image);

    /* make sure the whole range we are about to read really exists */
    if (start > available / 8 || count > available / 8 - start)
    {
        return STEG_ERR_FORMAT;
    }

    size_t position = start * 8;

    for (size_t i = 0; i < count; i++)
    {
        uint8_t rebuilt = 0;

        for (int bit = 0; bit < 8; bit++)
        {
            const uint8_t *source = image_byte_at(image, position);

            /*
             * shift what we have up one and drop the new bit in at the
             * bottom
             *
             * the cast to unsigned is there because c promotes the small
             * types to signed int before shifting, and the compiler is
             * right to complain about that changing the sign
             */
            unsigned int shifted = ((unsigned int)rebuilt << 1) |
                                   ((unsigned int)*source & 1u);

            rebuilt = (uint8_t)shifted;

            position++;
        }

        out[i] = rebuilt;
    }

    return STEG_OK;
}