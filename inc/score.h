/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Score Entry Header.
 */

/* types defined in this file */
typedef struct score Score;

#ifndef __SCORE_H__
#define __SCORE_H__

/*----------------------------------------------------------------------
 * Required Headers.
 */

/* standard C headers */
#include <stdio.h>

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/**
 * @struct score
 * An entry in a high score table.
 */
struct score {

    /*
     * Attributes
     */

    /** @var player The player who scored. */
    char player[14];

    /** @var scores The scores for each level. */
    int scores[12];

    /*
     * Methods
     */

    /**
     * Destroy the score when no longer needed.
     * @param score The score to destroy.
     */
    void (*destroy) (Score *score);

    /**
     * Clear the score data.
     * @param score The score to clear.
     */
    void (*clear) (Score *score);

    /**
     * Clone the score data.
     * @param  score The score to clone.
     * @return       The score clone.
     */
    Score *(*clone) (Score *score);

    /**
     * Write the score table. to an already-open file.
     * @param  score  The score table to read.
     * @param  output The output file handle.
     * @return        1 if successful, 0 on failure.
     */
    int (*write) (Score *score, FILE *output);

    /**
     * Read the score table from an already open file.
     * @param  score The score table to read.
     * @param  input The input file.
     * @return       1 if successful, 0 on failure.
     */
    int (*read) (Score *score, FILE *output);

    /**
     * Return a total of all the level scores.
     * @param  score The score to total.
     * @param  level The number of levels to count.
     * @return       The total of the level scores.
     */
    int (*total) (Score *score, int level);

};

/*----------------------------------------------------------------------
 * Top Level Function Declarations.
 */

/**
 * Create a new score.
 * @return The new score.
 */
Score *new_Score (void);

#endif
