#ifndef STATUS_H
#define STATUS_H

/*
 * status.h
 *
 * every function in this project returns one of these codes
 * so the cli can turn a failure into a useful exit status
 * instead of just printing something and returning 1
 */

typedef enum
{
    STEG_OK = 0,

    // the user passed bad arguments
    STEG_ERR_USAGE = 1,

    // something went wrong reading or writing a file
    STEG_ERR_IO = 2,

    // the file parsed but the contents are not what we expect
    STEG_ERR_FORMAT = 3,

    // the carrier is too small to hold this payload
    STEG_ERR_CAPACITY = 4,

    // wrong password or the payload failed its integrity check
    STEG_ERR_CRYPTO = 5,

    // malloc failed
    STEG_ERR_MEMORY = 6

} StegStatus;

// turns a status code into a short human readable string
const char *steg_status_string(StegStatus status);

#endif