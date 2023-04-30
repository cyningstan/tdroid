/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker 2022.
 * Level pack header.
 */

/* types defined in this file */
typedef struct levelpack LevelPack;

#ifdef __PACK_H__
#define __PACK_H__


/*----------------------------------------------------------------------
 * Data Definitions.
 */

/* required headers */
#include <stdio.h>
#include "level.h"
#include "score.h"

/**
 * @struct levelpack
 * A set of twelve levels.
 */
struct levelpack {

    /*
     * Attributes
     */

    /* @var name The level pack name, max 13 characters. */
    char name[14];

    /* @var set The twelve levels in the set. */
    Level *levels[12];

    /* @var scores The 12 best scores for this level pack. */
    Score *scores[12];

    /*
     * Methods
     */

    /**
     * Destroy the level pack when it is no longer needed.
     * @param levelpack The levelpack to destroy.
     */
    void (*destroy) (LevelPack *levelpack);

    /**
     * Clone the level pack.
     * @param  levelpack The level pack to clone.
     * @return            The new level pack.
     */
    Level *(*clone) (LevelPack *levelpack);

    /**
     * Write the level pack to an already open file.
     * @param  levelpack The level pack to write.
     * @param  output    The output file handle.
     * @return           1 if successful, 0 on failure.
     */
    int (*write) (LevelPack *levelpack, FILE *output);

    /**
     * Read the level pack from an already open file.
     * @param  levelpack The level pack to read.
     * @param  input     The input file handle.
     * @return           1 if successful, 0 on failure.
     */
    int (*read) (LevelPack *levelpack, FILE *output);

};

/*----------------------------------------------------------------------
 * Top Level Function Declarations.
 */

/**
 * LevelPack constructor.
 * @return The new level pack.
 */
LevelPack *new_LevelPack (void);

#endif
