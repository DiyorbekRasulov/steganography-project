#ifndef IMAGE_H
#define IMAGE_H

#include <stddef.h>
#include <stdint.h>

#include "status.h"

/*
 * image.h
 *
 * everything to do with getting a bmp off disk and back again
 *
 * a bmp is a header followed by rows of pixels
 * the header tells us where the pixels start, how wide the
 * image is, and how many bits each pixel uses
 *
 * the part that trips people up is that every row is padded
 * out to a multiple of four bytes, so the number of bytes in
 * a row is not just width times three
 */

typedef struct
{
    // the whole header exactly as it appeared in the file
    // this is not always 54 bytes so we allocate it
    uint8_t *header;

    // how many bytes of header there are before the pixels
    size_t header_size;

    // raw pixel bytes get stored here
    uint8_t *data;

    // image dimensions in pixels
    uint32_t width;
    uint32_t height;

    // usually 3 for rgb
    uint32_t channels;

    // bytes in one row including the padding at the end of it
    size_t row_stride;

    // total size of pixel data
    size_t data_size;

    // bmp stores rows bottom to top unless the height was negative
    int top_down;

} Image;

// loads a bmp image into memory
StegStatus load_bmp(const char *filename, Image **out_image);

// saves a modified bmp back to disk
StegStatus save_bmp(const char *filename, const Image *image);

// frees all allocated image memory
void free_image(Image *image);

/*
 * how many pixel bytes we are actually willing to touch
 *
 * the padding bytes at the end of each row are skipped because
 * other programs are allowed to rewrite them, so anything we
 * hid in there could quietly disappear
 */
size_t image_usable_bytes(const Image *image);

// hands back a pointer to usable byte number index
uint8_t *image_byte_at(const Image *image, size_t index);

#endif