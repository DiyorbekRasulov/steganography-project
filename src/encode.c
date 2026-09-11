#include "encode.h"
#include "utils.h"

/*
 * encode.c
 *
 * one of the two files that actually matter
 *
 * the caller has already checked capacity, but this checks again
 * because a function that walks a buffer should not trust somebody
 * else to have kept it in bounds
 */

StegStatus encode_data(Image *image, const uint8_t *blob, size_t blob_size)
{
    if (image == NULL || blob == NULL)
    {
        return STEG_ERR_USAGE;
    }

    size_t available = image_usable_bytes(image);

    /* one carrier byte holds one bit so we need eight per payload byte */
    if (blob_size > available / 8)
    {
        return STEG_ERR_CAPACITY;
    }

    size_t position = 0;

    for (size_t i = 0; i < blob_size; i++)
    {
        /* walk the byte from its highest bit down to its lowest */
        for (int bit = 0; bit < 8; bit++)
        {
            uint8_t value = get_bit(blob[i], bit);

            uint8_t *target = image_byte_at(image, position);

            *target = set_lsb(*target, value);

            position++;
        }
    }

    return STEG_OK;
}