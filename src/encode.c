#include <stdio.h>
#include "encode.h"

/*
 * encode.c
 *
 * Actual bit manipulation happens here.
 *
 * This is one of the core files of the project.
 *
 * Goals:
 * - modify image bytes safely
 * - preserve image quality
 * - support arbitrary file data later
 */

int encode_data(Image *image,
                uint8_t *secret_data,
                int secret_size)
{
    /*
     * TODO:
     * - loop through secret bytes
     * - extract bits
     * - modify image LSBs
     * - track image position
     * - avoid overflow
     */

    return 0;
}
