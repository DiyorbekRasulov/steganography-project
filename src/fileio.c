#include <stdio.h>
#include <stdlib.h>

#include "fileio.h"

/*
 * fileio.c
 *
 * generic binary file helpers, they do not know or care what the
 * bytes mean
 */

StegStatus read_file(const char *filename, uint8_t **out_data, size_t *out_size)
{
    if (filename == NULL || out_data == NULL || out_size == NULL)
    {
        return STEG_ERR_USAGE;
    }

    *out_data = NULL;
    *out_size = 0;

    FILE *file = fopen(filename, "rb");

    if (file == NULL)
    {
        return STEG_ERR_IO;
    }

    /* jump to the end to find out how big this is */
    if (fseek(file, 0, SEEK_END) != 0)
    {
        fclose(file);
        return STEG_ERR_IO;
    }

    long end = ftell(file);

    if (end < 0)
    {
        fclose(file);
        return STEG_ERR_IO;
    }

    if (fseek(file, 0, SEEK_SET) != 0)
    {
        fclose(file);
        return STEG_ERR_IO;
    }

    size_t size = (size_t)end;

    /* refuse anything silly before we hand the number to malloc */
    if (size > FILEIO_MAX_SIZE)
    {
        fclose(file);
        return STEG_ERR_CAPACITY;
    }

    /*
     * ask for one extra byte so an empty file still gets a real
     * pointer back rather than whatever malloc of zero decides to do
     */
    uint8_t *buffer = malloc(size + 1);

    if (buffer == NULL)
    {
        fclose(file);
        return STEG_ERR_MEMORY;
    }

    if (size > 0 && fread(buffer, 1, size, file) != size)
    {
        free(buffer);
        fclose(file);
        return STEG_ERR_IO;
    }

    fclose(file);

    *out_data = buffer;
    *out_size = size;

    return STEG_OK;
}

StegStatus write_file(const char *filename, const uint8_t *data, size_t size)
{
    if (filename == NULL || (data == NULL && size > 0))
    {
        return STEG_ERR_USAGE;
    }

    FILE *file = fopen(filename, "wb");

    if (file == NULL)
    {
        return STEG_ERR_IO;
    }

    /* a short write is a failure, not something to shrug at */
    if (size > 0 && fwrite(data, 1, size, file) != size)
    {
        fclose(file);
        return STEG_ERR_IO;
    }

    /* closing can fail too, that is when buffered data actually lands */
    if (fclose(file) != 0)
    {
        return STEG_ERR_IO;
    }

    return STEG_OK;
}