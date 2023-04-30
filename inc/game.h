/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Game Header.
 */

/* UIScreen type definition */
typedef struct game Game;

#ifndef __GAME_H__
#define __GAME_H__

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/* required headers */
#include "level.h"
#include "levelpak.h"
#include "action.h"

/**
 * struct @game
 * Model of a game in progress.
 */
struct game {

    /*
     * Attributes
     */

    /** @var filename The filename of the game. */
    char filename[13];

    /** @var player_name The name of the player. */
    char player[14];

    /** @var levelpackfile The file name of the level pack. */
    char levelpackfile[13];

    /** @var state The current state of the game. */
    int state;

    /** @var levelid The ID of the current level being played. */
    int levelid;

    /** @var turnno The current turn number. */
    int turnno;

    /** @var level_pack The level pack in use. */
    LevelPack *levelpack;

    /** @var level The current level as it stands. */
    Level *level;

    /** @var library
     * The order of actions in the library, including those not
     * currently available to the player.
     */
    Action *library[12];

    /** @var score The score entry with level scores so far. */
    Score *score;

    /*
     * Methods
     */
    
    /**
     * Destroy the game when it is no longer needed.
     * @param game The game to destroy.
     */
    void (*destroy) (Game *game);

    /**
     * Clear the game data.
     * @param game The game object to clear.
     */
    void (*clear) (Game *game);

    /**
     * Save the game. The filename is taken from the attributes.
     * @param  game The game to save.
     * @return      1 if successful, 0 on failure.
     */
    int (*save) (Game *game);

    /**
     * Load a game. The filename is taken from the attributes.
     * @param  game    The game to load.
     * @param  summary 0 to load the full game, 1 for summary only.
     * @return         1 if successful, 0 on failure.
     */
    int (*load) (Game *game, int summary);

    /**
     * Shuffle the actions at the start of a level.
     * @param  game The game whose actions are to be shuffled.
     */
    void (*shuffleactions) (Game *game);

    /**
     * End the current turn.
     * @param  game The game to advance.
     */
    int (*turn) (Game *game);

};

/*----------------------------------------------------------------------
 * Top-level Function Declarations.
 */

/**
 * Game contstructor function.
 * @return The new game.
 */
Game *new_Game (void);

#endif
