#include <stdio.h>
#include "steg.h"

/*
 * main.c
 *
 * This is the CLI entry point for the whole project.
 *
 * The website backend will eventually call this executable
 * with command line arguments.
 *
 * Example:
 * ./steg encode input.bmp secret.txt output.bmp
 * ./steg decode output.bmp extracted.txt
 *
 * Right now this file should:
 * 1. Parse arguments
 * 2. Validate user input
 * 3. Call high-level steg functions
 * 4. Print useful error messages
 *
 * Keep this file relatively clean.
 * Most actual logic should happen elsewhere.
 */

int main(int argc, char *argv[])
{
    printf("Steganography project starting...\n");

    /*
     * TODO:
     * - check argc
     * - support encode/decode commands
     * - call steg_encode()
     * - call steg_decode()
     */

    return 0;
}
