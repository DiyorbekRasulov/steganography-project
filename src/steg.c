#include <stdio.h>
#include "steg.h"
#include "image.h"
#include "encode.h"
#include "decode.h"
#include "fileio.h"

/*
 * steg.c
 *
 * This file acts like the "manager" of the project.
 *
 * The low-level bit manipulation should NOT happen here.
 *
 * Instead:
 * - load image
 * - load secret data
 * - call encoder
 * - save image
 *
 * Think of this as orchestration glue.
 */

int steg_encode(const char *input_image,
                const char *secret_file,
                const char *output_image)
{
    /*
     * TODO:
     * 1. Load image into memory
     * 2. Load secret file bytes
     * 3. Verify image capacity
     * 4. Call encode_data()
     * 5. Save modified image
     */

    return 0;
}

int steg_decode(const char *encoded_image,
                const char *output_file)
{
    /*
     * TODO:
     * 1. Load encoded image
     * 2. Extract hidden data
     * 3. Write extracted data to file
     */

    return 0;
}
