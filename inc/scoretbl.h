/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Score Table Header.
 */

/* types defined in this file */
typedef struct scoretable ScoreTable;

#ifndef __SCORETBL_H__
#define __SCORETBL_H__

/*----------------------------------------------------------------------
 * Required Headers.
 */

/* standard C headers */
#include <stdio.h>

/* project specific headers */
#include "score.h"

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/**
 * @struct scoretable
 * An entry in a high scoretable table.
 */
struct scoretable {

    /*
     * Attributes
     */

    /** @var scoretables The scoretables for each level. */
    Score *scores[12];

    /*
     * Methods
     */

    /**
     * Destroy the scoretable when no longer needed.
     * @param scoretable The score table to destroy.
     */
    void (*destroy) (ScoreTable *scoretable);

    /**
     * Clear the scoretable data.
     * @param scoretable The score table to clear.
     */
    void (*clear) (ScoreTable *scoretable);

    /**
     * Clone the score table.
     * @param  scoretable The score table to clone.
     * @return            The new score table.
     */
    ScoreTable *(*clone) (ScoreTable *scoretable);

    /**
     * Write the score table. to an already-open file.
     * @param  scoretable The score table to read.
     * @param  output     The output file handle.
     * @return            1 if successful, 0 on failure.
     */
    int (*write) (ScoreTable *scoretable, FILE *output);

    /**
     * Read the score table from an already open file.
     * @param  scoretable The score table to read.
     * @param  input      The input file.
     * @return            1 if successful, 0 on failure.
     */
    int (*read) (ScoreTable *scoretable, FILE *input);

    /**
     * Sort the score table.
     * @param scoretable The score table to sort.
     * @param level      Level for totalling scores.
     */
    void (*sort) (ScoreTable *scoretable, int level);

    /**
     * Insert a score into the right position on the score table.
     * @param  scoretable The score table to modify.
     * @param  score      The score to add to the table.
     * @param  level      The highest level to calculate.
     * @return            Position inserted, or -1 if not good enough.
     */
    int (*insert) (ScoreTable *scoretable, Score *score, int level);

};

/*----------------------------------------------------------------------
 * Top Level Function Declarations.
 */

/**
 * Create a new scoretable.
 * @return The new scoretable.
 */
ScoreTable *new_ScoreTable (void);

#endif
