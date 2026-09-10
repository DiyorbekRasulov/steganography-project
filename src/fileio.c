#include <stdio.h>
#include <stdlib.h>
#include "fileio.h"

/*
 * fileio.c
 *
 * Generic binary file utilities.
 *
 * IMPORTANT:
 * Always open hidden files in binary mode.
 *
 * Otherwise certain operating systems may
 * modify line endings automatically.
 */

uint8_t *read_file(const char *filename, int *size)
{
    /*
     * TODO:
     * - fopen()
     * - determine file size
     * - malloc buffer
     * - fread()
     */

    return NULL;
}

int write_file(const char *filename,
               uint8_t *data,
               int size)
{
    /*
     * TODO:
     * - fopen()
     * - fwrite()
     */

    return 0;
}
