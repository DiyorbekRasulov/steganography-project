#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image.h"
#include "utils.h"

/*
 * image.c
 *
 * bmp parsing lives here and nowhere else
 *
 * every number we read out of the header came from a file we
 * did not write, so all of it gets checked before it is used
 * to size an allocation or index into a buffer
 */

// the smallest header a bmp can have, 14 file header plus 40 info header
#define BMP_MIN_HEADER 54

// offsets of the fields we care about inside the header
#define BMP_OFFSET_PIXELS 10
#define BMP_OFFSET_WIDTH 18
#define BMP_OFFSET_HEIGHT 22
#define BMP_OFFSET_BPP 28

// refuse anything with a side longer than this, it is almost certainly junk
#define BMP_MAX_DIMENSION 65535u

/* works out how many bytes a row takes once padding is added */
static size_t row_stride_for(uint32_t width, uint32_t channels)
{
    size_t raw = (size_t)width * (size_t)channels;

    /* round up to the next multiple of four */
    return (raw + 3u) & ~(size_t)3u;
}

/* asks the operating system how big the file is */
static StegStatus file_size_of(FILE *file, size_t *out_size)
{
    if (fseek(file, 0, SEEK_END) != 0)
    {
        return STEG_ERR_IO;
    }

    long end = ftell(file);

    if (end < 0)
    {
        return STEG_ERR_IO;
    }

    if (fseek(file, 0, SEEK_SET) != 0)
    {
        return STEG_ERR_IO;
    }

    *out_size = (size_t)end;

    return STEG_OK;
}

StegStatus load_bmp(const char *filename, Image **out_image)
{
    if (filename == NULL || out_image == NULL)
    {
        return STEG_ERR_USAGE;
    }

    *out_image = NULL;

    // open the image in binary mode
    FILE *file = fopen(filename, "rb");

    if (file == NULL)
    {
        return STEG_ERR_IO;
    }

    /* we need the real file size to sanity check the header against */
    size_t file_size = 0;

    StegStatus status = file_size_of(file, &file_size);

    if (status != STEG_OK)
    {
        fclose(file);
        return status;
    }

    if (file_size < BMP_MIN_HEADER)
    {
        fclose(file);
        return STEG_ERR_FORMAT;
    }

    /* read the fixed part of the header first */
    uint8_t probe[BMP_MIN_HEADER];

    if (fread(probe, 1, BMP_MIN_HEADER, file) != BMP_MIN_HEADER)
    {
        fclose(file);
        return STEG_ERR_IO;
    }

    /* verify this is actually a bmp file */
    if (probe[0] != 'B' || probe[1] != 'M')
    {
        fclose(file);
        return STEG_ERR_FORMAT;
    }

    /* right now we only support 24 bit bmp files */
    uint16_t bits_per_pixel = read_le16(&probe[BMP_OFFSET_BPP]);

    if (bits_per_pixel != 24)
    {
        fclose(file);
        return STEG_ERR_FORMAT;
    }

    /*
     * where the pixels start is not always 54
     * extended info headers and colour profiles push it further out
     * so we keep whatever is in front of the pixels instead of
     * assuming a fixed size and losing the rest on save
     */
    uint32_t pixel_offset = read_le32(&probe[BMP_OFFSET_PIXELS]);

    if (pixel_offset < BMP_MIN_HEADER || (size_t)pixel_offset > file_size)
    {
        fclose(file);
        return STEG_ERR_FORMAT;
    }

    /* width and height are signed, a negative height means top down */
    int32_t signed_width = (int32_t)read_le32(&probe[BMP_OFFSET_WIDTH]);
    int32_t signed_height = (int32_t)read_le32(&probe[BMP_OFFSET_HEIGHT]);

    if (signed_width <= 0 || signed_height == 0)
    {
        fclose(file);
        return STEG_ERR_FORMAT;
    }

    int top_down = 0;

    if (signed_height < 0)
    {
        top_down = 1;
    }

    uint32_t width = (uint32_t)signed_width;

    /*
     * negating the minimum int32 would overflow so it gets rejected
     * rather than wrapped round into a positive number
     */
    if (signed_height == INT32_MIN)
    {
        fclose(file);
        return STEG_ERR_FORMAT;
    }

    uint32_t height = (uint32_t)(signed_height < 0 ? -signed_height : signed_height);

    if (width > BMP_MAX_DIMENSION || height > BMP_MAX_DIMENSION)
    {
        fclose(file);
        return STEG_ERR_FORMAT;
    }

    /* 24 bit rgb means 3 colour channels */
    uint32_t channels = 3;

    size_t row_stride = row_stride_for(width, channels);
    size_t data_size = row_stride * (size_t)height;

    /* the file has to actually be long enough to hold what it claims */
    if (data_size > file_size - (size_t)pixel_offset)
    {
        fclose(file);
        return STEG_ERR_FORMAT;
    }

    // allocate memory for the image struct
    Image *image = calloc(1, sizeof(Image));

    if (image == NULL)
    {
        fclose(file);
        return STEG_ERR_MEMORY;
    }

    image->width = width;
    image->height = height;
    image->channels = channels;
    image->row_stride = row_stride;
    image->data_size = data_size;
    image->header_size = (size_t)pixel_offset;
    image->top_down = top_down;

    image->header = malloc(image->header_size);

    if (image->header == NULL)
    {
        free(image);
        fclose(file);
        return STEG_ERR_MEMORY;
    }

    /* go back and read the header again, this time all of it */
    if (fseek(file, 0, SEEK_SET) != 0 ||
        fread(image->header, 1, image->header_size, file) != image->header_size)
    {
        free_image(image);
        fclose(file);
        return STEG_ERR_IO;
    }

    // allocate memory for the pixel data
    image->data = malloc(image->data_size);

    if (image->data == NULL)
    {
        free_image(image);
        fclose(file);
        return STEG_ERR_MEMORY;
    }

    /* read all pixel bytes into memory */
    if (fread(image->data, 1, image->data_size, file) != image->data_size)
    {
        free_image(image);
        fclose(file);
        return STEG_ERR_IO;
    }

    /* close the file because everything is now in memory */
    fclose(file);

    *out_image = image;

    return STEG_OK;
}

StegStatus save_bmp(const char *filename, const Image *image)
{
    if (filename == NULL || image == NULL)
    {
        return STEG_ERR_USAGE;
    }

    /* open output file in binary write mode */
    FILE *file = fopen(filename, "wb");

    if (file == NULL)
    {
        return STEG_ERR_IO;
    }

    /*
     * write the header straight through then the pixels straight
     * after it, no seeking, so there is never a gap of whatever
     * happened to be sitting in the output buffer
     */
    if (fwrite(image->header, 1, image->header_size, file) != image->header_size)
    {
        fclose(file);
        return STEG_ERR_IO;
    }

    if (fwrite(image->data, 1, image->data_size, file) != image->data_size)
    {
        fclose(file);
        return STEG_ERR_IO;
    }

    if (fclose(file) != 0)
    {
        return STEG_ERR_IO;
    }

    return STEG_OK;
}

void free_image(Image *image)
{
    /* avoid freeing a null pointer */
    if (image == NULL)
    {
        return;
    }

    free(image->header);
    free(image->data);

    /* then free the image struct itself */
    free(image);
}

size_t image_usable_bytes(const Image *image)
{
    if (image == NULL)
    {
        return 0;
    }

    /* width times channels is the row without its padding on the end */
    return (size_t)image->width * (size_t)image->channels * (size_t)image->height;
}

uint8_t *image_byte_at(const Image *image, size_t index)
{
    size_t bytes_per_row = (size_t)image->width * (size_t)image->channels;

    /* work out which row this index lands in and how far along it is */
    size_t row = index / bytes_per_row;
    size_t column = index % bytes_per_row;

    /* then skip past the padding of every row before it */
    return &image->data[row * image->row_stride + column];
}