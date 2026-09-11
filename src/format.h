#ifndef FORMAT_H
#define FORMAT_H

#include <stddef.h>
#include <stdint.h>

#include "status.h"

/*
 * format.h
 *
 * the decoder cannot just start pulling bits out of an image and
 * hope for the best, it has no idea whether anything is hidden in
 * there, where the data stops, or what it was to begin with
 *
 * so every payload gets a header written in front of it
 *
 * magic tells us this image is ours
 * version lets the format change later without breaking old files
 * flags say whether the payload was encrypted or compressed
 * type says whether it was text or a file
 * length says how much to read
 * crc says whether what we read is intact
 *
 * the layout on the wire is
 *
 * offset size field
 * 0      4    magic, the four letters STEG
 * 4      1    version
 * 5      1    flags
 * 6      1    payload type
 * 7      1    reserved, always zero for now
 * 8      8    payload length
 * 16     4    crc32 of the payload bytes only
 * 20     2    length of the original filename
 * 22     n    the filename itself, no terminator
 * 22 n   l    the payload
 */

#define FORMAT_MAGIC_0 'S'
#define FORMAT_MAGIC_1 'T'
#define FORMAT_MAGIC_2 'E'
#define FORMAT_MAGIC_3 'G'

#define FORMAT_VERSION 1

// the fixed part of the header, before any filename
#define FORMAT_HEADER_SIZE 22

// longest filename we are prepared to store
#define FORMAT_MAX_NAME 255

// flag bits
#define FORMAT_FLAG_ENCRYPTED 0x01
#define FORMAT_FLAG_COMPRESSED 0x02

typedef enum
{
    PAYLOAD_RAW = 0,
    PAYLOAD_TEXT = 1,
    PAYLOAD_FILE = 2

} PayloadType;

typedef struct
{
    uint8_t version;
    uint8_t flags;
    uint8_t payload_type;

    uint64_t payload_length;

    uint32_t crc;

    uint16_t name_length;

    char name[FORMAT_MAX_NAME + 1];

} FormatHeader;

/*
 * builds the header plus payload into one buffer ready to embed
 *
 * the caller owns the buffer that comes back and has to free it
 */
StegStatus format_serialize(const FormatHeader *header,
                            const uint8_t *payload,
                            size_t payload_size,
                            uint8_t **out_blob,
                            size_t *out_blob_size);

/*
 * reads the fixed header out of a buffer and checks it makes sense
 *
 * limit is how many payload bytes could possibly fit in the carrier
 * so a corrupted length field cannot talk us into a huge allocation
 */
StegStatus format_parse_header(const uint8_t *buffer,
                               size_t buffer_size,
                               uint64_t limit,
                               FormatHeader *out_header);

// everything the header occupies including the filename
size_t format_total_header_size(const FormatHeader *header);

#endif