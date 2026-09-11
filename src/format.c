#include <stdlib.h>
#include <string.h>

#include "format.h"
#include "utils.h"

/*
 * format.c
 *
 * nothing in here writes a struct straight to disk
 *
 * the compiler is free to put padding between struct members and
 * to store numbers in whatever byte order the machine likes, so a
 * struct dumped on one machine may not read back on another
 * every field goes out one byte at a time instead
 */

size_t format_total_header_size(const FormatHeader *header)
{
    return (size_t)FORMAT_HEADER_SIZE + (size_t)header->name_length;
}

StegStatus format_serialize(const FormatHeader *header,
                            const uint8_t *payload,
                            size_t payload_size,
                            uint8_t **out_blob,
                            size_t *out_blob_size)
{
    if (header == NULL || out_blob == NULL || out_blob_size == NULL)
    {
        return STEG_ERR_USAGE;
    }

    if (payload == NULL && payload_size > 0)
    {
        return STEG_ERR_USAGE;
    }

    if (header->name_length > FORMAT_MAX_NAME)
    {
        return STEG_ERR_USAGE;
    }

    size_t head = format_total_header_size(header);
    size_t total = head + payload_size;

    uint8_t *blob = malloc(total);

    if (blob == NULL)
    {
        return STEG_ERR_MEMORY;
    }

    /* magic goes first so the decoder can bail out quickly */
    blob[0] = FORMAT_MAGIC_0;
    blob[1] = FORMAT_MAGIC_1;
    blob[2] = FORMAT_MAGIC_2;
    blob[3] = FORMAT_MAGIC_3;

    blob[4] = header->version;
    blob[5] = header->flags;
    blob[6] = header->payload_type;

    /* reserved byte, kept zero so a later version can use it */
    blob[7] = 0;

    write_le64(&blob[8], (uint64_t)payload_size);

    /* the checksum covers the payload only, not the header */
    write_le32(&blob[16], crc32_compute(payload, payload_size));

    write_le16(&blob[20], header->name_length);

    if (header->name_length > 0)
    {
        memcpy(&blob[FORMAT_HEADER_SIZE], header->name, header->name_length);
    }

    if (payload_size > 0)
    {
        memcpy(&blob[head], payload, payload_size);
    }

    *out_blob = blob;
    *out_blob_size = total;

    return STEG_OK;
}

StegStatus format_parse_header(const uint8_t *buffer,
                               size_t buffer_size,
                               uint64_t limit,
                               FormatHeader *out_header)
{
    if (buffer == NULL || out_header == NULL)
    {
        return STEG_ERR_USAGE;
    }

    if (buffer_size < FORMAT_HEADER_SIZE)
    {
        return STEG_ERR_FORMAT;
    }

    /* if the magic is wrong there is no hidden data here at all */
    if (buffer[0] != FORMAT_MAGIC_0 ||
        buffer[1] != FORMAT_MAGIC_1 ||
        buffer[2] != FORMAT_MAGIC_2 ||
        buffer[3] != FORMAT_MAGIC_3)
    {
        return STEG_ERR_FORMAT;
    }

    memset(out_header, 0, sizeof(*out_header));

    out_header->version = buffer[4];

    /* a newer version may lay the rest of the bytes out differently */
    if (out_header->version != FORMAT_VERSION)
    {
        return STEG_ERR_FORMAT;
    }

    out_header->flags = buffer[5];
    out_header->payload_type = buffer[6];

    /* the reserved byte has to be zero or something has gone wrong */
    if (buffer[7] != 0)
    {
        return STEG_ERR_FORMAT;
    }

    out_header->payload_length = read_le64(&buffer[8]);
    out_header->crc = read_le32(&buffer[16]);
    out_header->name_length = read_le16(&buffer[20]);

    if (out_header->name_length > FORMAT_MAX_NAME)
    {
        return STEG_ERR_FORMAT;
    }

    /*
     * this is the important check
     *
     * the length came out of the image and the image could have been
     * edited by anyone, so it gets compared against what the carrier
     * could physically hold before anybody calls malloc with it
     */
    if (out_header->payload_length > limit)
    {
        return STEG_ERR_FORMAT;
    }

    return STEG_OK;
}