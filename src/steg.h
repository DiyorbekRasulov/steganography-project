#ifndef STEG_H
#define STEG_H

/*
 * steg.h
 *
 * High-level interface for the entire project.
 *
 * This layer coordinates:
 * - image loading
 * - encoding
 * - decoding
 * - writing output files
 *
 * main.c should mostly interact with THIS file.
 */

int steg_encode(const char *input_image,
                const char *secret_file,
                const char *output_image);

int steg_decode(const char *encoded_image,
                const char *output_file);

#endif
