/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker 2022.
 * Utilities module.
 */

/*----------------------------------------------------------------------
 * Headers
 */

/* ANSI C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*----------------------------------------------------------------------
 * Public Level Routines.
 */

/**
 * Write an integer as a byte to an already open output file.
 * @param  value  A pointer to the integer variable to write.
 * @param  output The output file handle.
 * @return        1 if successful, 0 if not.
 */
int writeint (int *value, FILE *output)
{
    unsigned char c; /* character to read */
    c = (char) (*value & 0xff);
    return fwrite (&c, 1, 1, output);
}

/**
 * Write a string to an already open output file.
 * @param  value  A pointer to the string buffer to write.
 * @param  output The output file handle.
 * @return        1 if successful, 0 if not.
 */
int writestring (char *value, FILE *output)
{
    unsigned char length; /* length byte */
    length = (char) strlen (value);
    if (! (fwrite (&length, 1, 1, output)))
	return 0;
    if (length)
	return fwrite (value, (int) length, 1, output);
    return 1;
}

/**
 * Read a byte from an already open input file
 * and store it in an integer variable.
 * @param  value A pointer to the integer variable to store into.
 * @param  input The input file handle.
 * @return       1 if successful, 0 if not.
 */
int readint (int *value, FILE *input)
{
    unsigned char c; /* character to read */
    if (! (fread (&c, 1, 1, input)))
	return 0;
    *value = (int) c;
    return 1;
}

/**
 * Read a string from an already open input file
 * and store it in a string buffer.
 * @param  value A pointer to the string buffer to store into.
 * @param  input The input file handle.
 * @return       1 if successful, 0 if not.
 */
int readstring (char *value, FILE *input)
{
    unsigned char length; /* length byte */
    if (! (fread (&length, 1, 1, input)))
	return 0;
    if (length &&
	! (fread (value, length, 1, input)))
	return 0;
    value[(int) length] = '\0';
    return 1;
}

