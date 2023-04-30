/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Score Entry Module.
 */

/*----------------------------------------------------------------------
 * Headers.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* project specific headers */
#include "score.h"
#include "fatal.h"
#include "utils.h"


/*----------------------------------------------------------------------
 * Public Level Function Definitions.
 */

/**
 * Destroy the score when no longer needed.
 * @param score The score to destroy.
 */
static void destroy (Score *score)
{
    if (score)
	free (score);
}

/**
 * Clear the score data.
 * @param score The score to clear.
 */
static void clear (Score *score)
{
    int c; /* generic counter */
    *score->player = '\0';
    for (c = 0; c < 12; ++c)
	score->scores[c] = 0;
}

/**
 * Clone the score data.
 * @param  score The score to clone.
 * @return       The score clone.
 */
static Score *clone (Score *score)
{
    Score *newscore; /* the new score */
    int c; /* level counter */
    newscore = new_Score ();
    strcpy (newscore->player, score->player);
    for (c = 0; c < 12; ++c)
	newscore->scores[c] = score->scores[c];
    return newscore;
}

/**
 * Write the score table. to an already-open file.
 * @param  score  The score table to read.
 * @param  output The output file handle.
 * @return        1 if successful, 0 on failure.
 */
static int write (Score *score, FILE *output)
{
    int r, /* return value */
	c; /* generic counter */
    r = writestring (score->player, output);
    for (c = 0; r && c < 12; c++)
	r = r && writeint (&score->scores[c], output);
    return r;
}

/**
 * Read the score table from an already open file.
 * @param  score The score table to read.
 * @param  input The input file.
 * @return       1 if successful, 0 on failure.
 */
static int read (Score *score, FILE *input)
{
    int r, /* return value */
	c; /* generic counter */
    r = readstring (score->player, input);
    for (c = 0; r && c < 12; c++)
	r = r && readint (&score->scores[c], input);
    return r;
}

/**
 * Return a total of all the level scores.
 * @param  score The score to total.
 * @param  level The number of levels to count.
 * @return       The total of the level scores.
 */
static int total (Score *score, int level)
{
    int scoretotal, /* cumulative total */
	c; /* general counter */
    scoretotal = 0;
    for (c = 0; c < level; ++c)
	scoretotal += score->scores[c];
    return scoretotal;
}

/*----------------------------------------------------------------------
 * Top Level Function Definitions.
 */

/**
 * Create a new score.
 * @return The new score.
 */
Score *new_Score (void)
{
    Score *score; /* the new score */

    /* reserve memory */
    if (! (score = malloc (sizeof (Score))))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* initialise attributes */
    clear (score);

    /* initialise methods */
    score->destroy = destroy;
    score->clear = clear;
    score->clone = clone;
    score->write = write;
    score->read = read;
    score->total = total;

    /* return the new score */
    return score;
}

