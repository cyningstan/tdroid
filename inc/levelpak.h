/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Level Pack Header.
 */

/* types defined in this file */
typedef struct levelpack LevelPack;
typedef struct score Score;

#ifndef __LEVELPAK_H__
#define __LEVELPAK_H__

/* required headers */
#include "level.h"
#include "scoretbl.h"

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/**
 * @struct levelpack
 * A pack of twelve levels.
 */
struct levelpack {

    /*
     * Attributes
     */

    /** @var filename The file the level pack is stored in. */
    char filename[13];

    /** @var name The level pack's display name. */
    char name[14];

    /** @var levels The twelve levels in the level pack. */
    Level *levels[12];

    /** @var scores The high score table. */
    ScoreTable *scoretable;

    /*
     * Methods
     */

    /**
     * Destroy the level pack when no longer needed.
     * @param levelpack The pack to destroy.
     */
    void (*destroy) (LevelPack *levelpack);

    /**
     * Clear the level pack data.
     * @param levelpack The pack to clear.
     */
    void (*clear) (LevelPack *levelpack);

    /**
     * Save the level pack. Filename is in the attributes.
     * @param  levelpack The pack to save.
     * @return           1 if successful, 0 on failure.
     */
    int (*save) (LevelPack *levelpack);

    /**
     * Save the high score table. Filename is in the attributes.
     * @param  levelpack The pack to save.
     * @return           1 if successful, 0 on failure.
     */
    int (*savescores) (LevelPack *levelpack);

    /**
     * Load the level pack. Filename is in the attributes.
     * @param  levelpack The pack to load.
     * @param  summary   1 if only a summary is to be loaded.
     * @return           1 if successful, 0 on failure.
     */
    int (*load) (LevelPack *levelpack, int summary);

};

/*----------------------------------------------------------------------
 * Top Level Function Declarations.
 */

/**
 * Create a new level pack.
 * @return The new level pack.
 */
LevelPack *new_LevelPack (void);

#endif
