#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>

/*
 * utils.h
 *
 * small helpers that more than one file needs
 *
 * the read and write helpers exist because bmp stores its
 * numbers little endian, and casting a pointer into the
 * middle of a byte array is undefined behaviour, so every
 * multi byte field goes through these instead
 */

// pulls a 2 byte little endian value out of a byte buffer
uint16_t read_le16(const uint8_t *buffer);

// pulls a 4 byte little endian value out of a byte buffer
uint32_t read_le32(const uint8_t *buffer);

// pulls an 8 byte little endian value out of a byte buffer
uint64_t read_le64(const uint8_t *buffer);

// writes a 2 byte value into a buffer little endian
void write_le16(uint8_t *buffer, uint16_t value);

// writes a 4 byte value into a buffer little endian
void write_le32(uint8_t *buffer, uint32_t value);

// writes an 8 byte value into a buffer little endian
void write_le64(uint8_t *buffer, uint64_t value);

// standard crc32 so the decoder can tell if the payload survived
uint32_t crc32_compute(const uint8_t *data, size_t size);

// pulls bit number position out of a byte, counting from the left
uint8_t get_bit(uint8_t byte, int position);

// returns byte with its least significant bit replaced by bit
uint8_t set_lsb(uint8_t byte, uint8_t bit);

// prints a byte as eight ones and zeros, useful while debugging
void print_binary(unsigned char byte);

#endif
