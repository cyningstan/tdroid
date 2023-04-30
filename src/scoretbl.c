/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Score Table Module.
 */

/*----------------------------------------------------------------------
 * Headers.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* project specific headers */
#include "scoretbl.h"
#include "fatal.h"
#include "score.h"
#include "utils.h"

/*----------------------------------------------------------------------
 * Public Level Function Definitions.
 */

/*
 * Methods
 */

/**
 * Destroy the scoretable when no longer needed.
 * @param scoretable The score table to destroy.
 */
static void destroy (ScoreTable *scoretable)
{
    int c; /* general counter */
    if (scoretable) {
	for (c = 0; c < 12; ++c)
	    scoretable->scores[c]->destroy (scoretable->scores[c]);
	free (scoretable);
    }
}

/**
 * Clone the score table.
 * @param  scoretable The score table to clone.
 * @return            The new score table.
 */
static ScoreTable *clone (ScoreTable *scoretable)
{
    ScoreTable *newtable; /* new score table */
    int s, /* score line counter */
	l; /* level counter */

    /* Create the new score table */
    newtable = new_ScoreTable ();

    /* copy the attributes */
    for (s = 0; s < 12; ++s) {
	strcpy (newtable->scores[s]->player,
		scoretable->scores[s]->player);
	for (l = 0; l < 12; ++l)
	    newtable->scores[s]->scores[l] =
		scoretable->scores[s]->scores[l];
    }

    /* return the new score table */
    return newtable;	
}

/**
 * Clear the scoretable data.
 * @param scoretable The score table to clear.
 */
static void clear (ScoreTable *scoretable)
{
    int c; /* general counter */
    for (c = 0; c < 12; ++c)
	scoretable->scores[c]->clear (scoretable->scores[c]);
}

/**
 * Write the score table. to an already-open file.
 * @param  scoretable The score table to read.
 * @param  output     The output file handle.
 * @return            1 if successful, 0 on failure.
 */
static int write (ScoreTable *scoretable, FILE *output)
{
    int c, /* general counter */
	r = 1; /* return value */
    for (c = 0; c < 12; ++c)
	r = r && scoretable->scores[c]->write (scoretable->scores[c],
					       output);
    return r;
}

/**
 * Read the score table from an already open file.
 * @param  scoretable The score table to read.
 * @param  input      The input file.
 * @return            1 if successful, 0 on failure.
 */
static int read (ScoreTable *scoretable, FILE *input)
{
    int c, /* general counter */
	r = 1; /* return value */
    for (c = 0; c < 12; ++c)
	r = r && scoretable->scores[c]->read (scoretable->scores[c],
					      input);
    return r;
}

/**
 * Sort the score table.
 * @param scoretable The score table to sort.
 */
static void sort (ScoreTable *scoretable, int level)
{
    Score *tmp; /* temporary score pointer */
    int sorted, /* flag to indicate scores sorted */
	c, /* score counter */
	total1, /* total of first score line */
	total2; /* total of second score line */

    /* loop until sorted */
    do {

	/* assume sorted */
	sorted = 1;

	/* sweep through scores */
	for (c = 0; c < 11; ++c) {

	    /* get totals */
	    total1 = scoretable->scores[c]->total
		(scoretable->scores[c], level);
	    total2 = scoretable->scores[c + 1]->total
		(scoretable->scores[c + 1], level);

	    /* swap if in the wrong order */
	    if ((total1 > total2 && total2 != 0) ||
		(total1 == 0 && total2 != 0)) {
		sorted = 0;
		tmp = scoretable->scores[c];
		scoretable->scores[c] = scoretable->scores[c + 1];
		scoretable->scores[c + 1] = tmp;
	    }
	}
    } while (! sorted);
}

/**
 * Insert a score into the right position on the score table.
 * @param  scoretable The score table to modify.
 * @param  score      The score to add to the table.
 * @param  level      The last level to consider in the total.
 * @return            Position inserted, or -1 if not good enough.
 */
static int insert (ScoreTable *scoretable, Score *score, int level)
{
    int playertotal, /* total of player level scores */
	insertionpoint, /* where the score is to be inserted */
	c; /* general counter */
    Score *tablescore, /* a score in the existing table */
	*scoreclone; /* a clone of the incoming score */

    /* don't insert if there isn't a name */
    if (! *score->player)
	return -1;

    /* calculate player total */
    scoreclone = score->clone (score);
    playertotal = scoreclone->total (scoreclone, level + 1);

    /* find insertion point */
    insertionpoint = -1;
    for (c = 0; insertionpoint == -1 && c < 12; ++c) {
	tablescore = scoretable->scores[c];
	if (! *tablescore->player ||
	    tablescore->total (tablescore, level + 1) > playertotal)
	    insertionpoint = c;
    }

    /* return if the score isn't good enough */
    if (insertionpoint == -1)
	return -1;

    /* make room for the new score */
    scoretable->scores[11]->destroy (scoretable->scores[11]);
    for (c = 11; c > insertionpoint; --c)
	scoretable->scores[c] = scoretable->scores[c - 1];

    /* insert the new score and return */
    scoretable->scores[insertionpoint] = scoreclone;
    return insertionpoint;
}

/*----------------------------------------------------------------------
 * Top Level Function Definitions.
 */

/**
 * Create a new scoretable.
 * @return The new scoretable.
 */
ScoreTable *new_ScoreTable (void)
{
    ScoreTable *scoretable; /* the new scoretable */
    int c; /* general counter */

    /* reserve memory */
    if (! (scoretable = malloc (sizeof (ScoreTable))))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* initialise attributes */
    for (c = 0; c < 12; ++c)
	if (! (scoretable->scores[c] = new_Score ()))
	    fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* initialise methods */
    scoretable->destroy = destroy;
    scoretable->clear = clear;
    scoretable->clone = clone;
    scoretable->write = write;
    scoretable->read = read;
    scoretable->sort = sort;
    scoretable->insert = insert;

    /* return the new scoretable */
    return scoretable;
}
