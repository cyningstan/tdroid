/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker 2022.
 * Utilities header.
 */

#ifndef __UTILS_H__
#define __UTILS_H__

/**
 * Write an integer as a byte to an already open output file.
 * @param  value  A pointer to the integer variable to write.
 * @param  output The output file handle.
 * @return        1 if successful, 0 if not.
 */
int writeint (int *value, FILE *output);

/**
 * Write a string to an already open output file.
 * @param  value  A pointer to the string buffer to write.
 * @param  output The output file handle.
 * @return        1 if successful, 0 if not.
 */
int writestring (char *value, FILE *output);

/**
 * Read a byte from an already open input file
 * and store it in an integer variable.
 * @param  value A pointer to the integer variable to store into.
 * @param  input The input file handle.
 * @return       1 if successful, 0 if not.
 */
int readint (int *value, FILE *input);

/**
 * Read a string from an already open input file
 * and store it in a string buffer.
 * @param  value A pointer to the string buffer to store into.
 * @param  input The input file handle.
 * @return       1 if successful, 0 if not.
 */
int readstring (char *value, FILE *input);

#endif
