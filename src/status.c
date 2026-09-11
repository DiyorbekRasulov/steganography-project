#include "status.h"

/*
 * status.c
 *
 * one place to turn a code into words so the same failure never
 * gets described two different ways in two different files
 */

const char *steg_status_string(StegStatus status)
{
    switch (status)
    {
        case STEG_OK:
            return "ok";

        case STEG_ERR_USAGE:
            return "bad arguments";

        case STEG_ERR_IO:
            return "could not read or write the file";

        case STEG_ERR_FORMAT:
            return "the file is not in a format this tool understands";

        case STEG_ERR_CAPACITY:
            return "the carrier is not big enough for this payload";

        case STEG_ERR_CRYPTO:
            return "wrong password or the hidden data is damaged";

        case STEG_ERR_MEMORY:
            return "ran out of memory";

        default:
            return "unknown error";
    }
}