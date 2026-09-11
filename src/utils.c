#include <stdio.h>
#include <string.h>

#include "utils.h"

/*
 * utils.c
 *
 * nothing in here knows anything about images or payloads
 * it is all pure byte and bit work so it is easy to test
 */

uint16_t read_le16(const uint8_t *buffer)
{
    /* build the value by shifting each byte into place */
    return (uint16_t)((uint16_t)buffer[0] |
                      ((uint16_t)buffer[1] << 8));
}

uint32_t read_le32(const uint8_t *buffer)
{
    return (uint32_t)buffer[0] |
           ((uint32_t)buffer[1] << 8) |
           ((uint32_t)buffer[2] << 16) |
           ((uint32_t)buffer[3] << 24);
}

uint64_t read_le64(const uint8_t *buffer)
{
    uint64_t value = 0;

    /* walk the bytes backwards so the last byte ends up highest */
    for (int i = 7; i >= 0; i--)
    {
        value = (value << 8) | (uint64_t)buffer[i];
    }

    return value;
}

void write_le16(uint8_t *buffer, uint16_t value)
{
    buffer[0] = (uint8_t)(value & 0xFF);
    buffer[1] = (uint8_t)((value >> 8) & 0xFF);
}

void write_le32(uint8_t *buffer, uint32_t value)
{
    for (int i = 0; i < 4; i++)
    {
        /* peel off the lowest byte each time round */
        buffer[i] = (uint8_t)((value >> (i * 8)) & 0xFF);
    }
}

void write_le64(uint8_t *buffer, uint64_t value)
{
    for (int i = 0; i < 8; i++)
    {
        buffer[i] = (uint8_t)((value >> (i * 8)) & 0xFF);
    }
}

/*
 * crc32 using the normal reflected ieee polynomial
 *
 * the table gets built once on the first call so we are not
 * doing eight shifts per byte every single time
 */
static uint32_t crc_table[256];
static int crc_table_ready = 0;

static void crc32_build_table(void)
{
    for (uint32_t i = 0; i < 256; i++)
    {
        uint32_t value = i;

        for (int bit = 0; bit < 8; bit++)
        {
            /* if the low bit is set we xor in the polynomial */
            if (value & 1u)
            {
                value = (value >> 1) ^ 0xEDB88320u;
            }
            else
            {
                value = value >> 1;
            }
        }

        crc_table[i] = value;
    }

    crc_table_ready = 1;
}

uint32_t crc32_compute(const uint8_t *data, size_t size)
{
    if (crc_table_ready == 0)
    {
        crc32_build_table();
    }

    /* the standard says start at all ones and invert at the end */
    uint32_t crc = 0xFFFFFFFFu;

    for (size_t i = 0; i < size; i++)
    {
        uint8_t index = (uint8_t)((crc ^ data[i]) & 0xFFu);

        crc = (crc >> 8) ^ crc_table[index];
    }

    return crc ^ 0xFFFFFFFFu;
}

uint8_t get_bit(uint8_t byte, int position)
{
    /*
     * position 0 is the leftmost bit so we shift from the top down
     *
     * the value gets widened to unsigned first because c promotes
     * a uint8_t to signed int before shifting it, and mixing that
     * with an unsigned mask is exactly the kind of quiet sign
     * change that -Wconversion exists to catch
     */
    unsigned int widened = (unsigned int)byte;

    return (uint8_t)((widened >> (7u - (unsigned int)position)) & 1u);
}

uint8_t set_lsb(uint8_t byte, uint8_t bit)
{
    /* clear the bottom bit then or the new one in */
    return (uint8_t)((byte & 0xFEu) | (bit & 1u));
}

void print_binary(unsigned char byte)
{
    for (int i = 0; i < 8; i++)
    {
        printf("%u", (unsigned)get_bit((uint8_t)byte, i));
    }

    printf("\n");
}