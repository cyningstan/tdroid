/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker 2022.
 * Level header.
 */

/* types defined in this file */
typedef struct level Level;

#ifndef __LEVEL_H__
#define __LEVEL_H__

/*----------------------------------------------------------------------
 * Required Headers.
 */

/* standard C headers */
#include <stdio.h>

/* project specific headers */
#include "cell.h"
#include "robot.h"
#include "item.h"

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/**
 * @struct level
 * Model of a game level.
 */
struct level {

    /*
     * Attributes
     */

    /** @var cells Pointers to cell types, one per cell. */
    Cell *cells[192];

    /** @var items Items on each cell. */
    Item *items[192];

    /** @var robots Pointers to robots on each cell. */
    Robot *robots[192];

    /** @var readers The number of card readers on the level. */
    int readers;

    /** @var cards The number of data cards placed. */
    int cards;

    /** @var spawners The number of robots to be placed. */
    int spawners;

    /** @var robotcount The number of robots present. */
    int robotcount;

    /** @var turns The number of turns taken on this level. */
    int turns;

    /*
     * Methods
     */

    /**
     * Destroy the level when it is no longer needed.
     * @param level The level to destroy.
     */
    void (*destroy) (Level *level);

    /**
     * Clone the level.
     * @param  level The level to clone.
     * @return       The new level.
     */
    Level *(*clone) (Level *level);

    /**
     * Clear the level.
     * @param  level The level to clear.
     */
    void (*clear) (Level *level);

    /**
     * Write the level to an already open file.
     * @param  level  The level to write.
     * @param  output The output file handle.
     * @return        1 if successful, 0 on failure.
     */
    int (*write) (Level *level, FILE *output);

    /**
     * Read the level from an already open file.
     * @param  level The level to read.
     * @param  input The input file handle.
     * @return       1 if successful, 0 on failure.
     */
    int (*read) (Level *level, FILE *output);

};

/*----------------------------------------------------------------------
 * Top-level Function Declarations.
 */

/**
 * Level contstructor function.
 * @return The new level.
 */
Level *new_Level (void);

#endif
