#ifndef FILEIO_H
#define FILEIO_H

#include <stddef.h>
#include <stdint.h>

#include "status.h"

/*
 * fileio.h
 *
 * reading and writing whole files as raw bytes
 *
 * everything opens in binary mode, if it did not then windows
 * would helpfully turn every 0x0A byte in a payload into 0x0D 0x0A
 * and the file that came back out would not match the one that
 * went in
 */

// biggest payload we will read off disk, sixty four megabytes
#define FILEIO_MAX_SIZE (64u * 1024u * 1024u)

// reads a whole file into a buffer the caller has to free
StegStatus read_file(const char *filename, uint8_t **out_data, size_t *out_size);

// writes a buffer out as a file
StegStatus write_file(const char *filename, const uint8_t *data, size_t size);

#endif