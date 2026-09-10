#ifndef FILEIO_H
#define FILEIO_H

#include <stdint.h>

/*
 * fileio.h
 *
 * Handles generic file reading/writing.
 *
 * Used for:
 * - secret text files
 * - arbitrary binary files later
 */

uint8_t *read_file(const char *filename, int *size);

int write_file(const char *filename,
               uint8_t *data,
               int size);

#endif
