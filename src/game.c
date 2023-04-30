/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Game module.
 */

/*----------------------------------------------------------------------
 * Headers
 */

/* ANSI C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* project-specific headers */
#include "tdroid.h"
#include "fatal.h"
#include "game.h"
#include "config.h"
#include "levelpak.h"
#include "level.h"
#include "item.h"
#include "action.h"
#include "uiscreen.h"
#include "utils.h"

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/** @var config A pointer to the Config library object. */
static Config *config = NULL;

/*----------------------------------------------------------------------
 * Level 1 Private Functions.
 */

/**
 * Create the other objects that are part of the game.
 * @param game The game whose objects are initialise.
 */
static void createobjects (Game *game)
{
    int c; /* general counter */
    if (! (game->levelpack = new_LevelPack ()))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
    if (! (game->level = new_Level ()))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
    for (c = 0; c < 12; ++c)
	game->library[c] = get_Action (1 + c);
    if (! (game->score = new_Score ()))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
}

/**
 * Destroy any subsidiary objects for this game.
 * @param game The game to clear.
 */
static void destroyobjects (Game *game)
{
    int c; /* general counter */

    /* free up the memory */
    if (game->levelpack)
	game->levelpack->destroy (game->levelpack);
    if (game->level)
	game->level->destroy (game->level);
    if (game->score)
	game->score->destroy (game->score);

    /* clear the pointers */
    game->levelpack = NULL;
    game->level = NULL;
    for (c = 0; c < 12; ++c)
	game->library[c] = NULL;
}

/**
 * Initialise the attributes of a game object.
 * @param game The game to initialise.
 */
static void initialiseattributes (Game *game)
{
    *game->filename = '\0';
    *game->player = '\0';
    strcpy (game->levelpackfile, config->levelpackfile);
    game->state = STATE_NEWGAME;
    game->levelid = 0;
    game->turnno = 0;
}

/**
 * Check if a level is complete.
 * @param game The game to check.
 */
static int checkcomplete (Game *game)
{
    int readerswithoutcards, /* number of readers needing cards */
	c; /* cell counter */
    Level *level; /* pointer to current level */

    /* initialise convenience variables */
    level = game->level;

    /* do all card readers have a card? */
    readerswithoutcards = 0;
    for (c = 0; c < 192; ++c)
	if (level->cells[c]->type == CELL_READER &&
	    (! level->items[c] ||
	     level->items[c]->type != ITEM_CARD))
	    ++readerswithoutcards;
    if (! readerswithoutcards)
	return 1;

    /* no completion detected */
    return 0;
}

/**
 * Check if a level is failed.
 * @param game The game to check.
 */
static int checkfailed (Game *game)
{
    int playerrobots = 0, /* number of player robots remaining */
	datacards = 0, /* number of data cards remaining */
	readers = 0, /* number of readers on the map */
	c; /* general purpose counters */
    Level *level; /* pointer to current level */

    /* initialise convenience variables */
    level = game->level;

    /* count up all the things that calculate level failure */
    for (c = 0; c < 192; ++c) {
	if (level->robots[c] && level->robots[c]->type != ROBOT_GUARD)
	    ++playerrobots;
	if (level->items[c] && level->items[c]->type == ITEM_CARD)
	    ++datacards;
	if (level->cells[c]->type == CELL_READER)
	    ++readers;
    }

    /* return level failure */
    if (playerrobots == 0)
	return 1; /* no player robots left */
    if (datacards < readers)
	return 1; /* data card(s) destroyed */
    return 0;
}

/*----------------------------------------------------------------------
 * Public Method Function Definitions.
 */

/**
 * Destroy the game when it is no longer needed.
 * @param game The game to destroy.
 */
static void destroy (Game *game)
{
    if (game) {
	destroyobjects (game);
	free (game);
    }
}

/**
 * Clear the game data.
 * @param game The game object to clear.
 */
static void clear (Game *game)
{
    game->levelpack->clear (game->levelpack);
    game->level->clear (game->level);
    /* clear or reinitialise the library actions here? */
    game->score->clear (game->score);
    initialiseattributes (game);
}

/**
 * Save the game. The filename is taken from the attributes.
 * @param  game The game to save.
 * @return      1 if successful, 0 on failure.
 */
static int save (Game *game)
{
    FILE *output; /* the output file */
    int r = 1, /* return value */
	c; /* general counter */

    /* open the output file */
    if (! (output = fopen (game->filename, "wb")))
	return 0;

    /* write the game header */
    r = r && fwrite ("TDR100G", 8, 1, output);
    
    /* write the basic information */
    r = r &&
	writestring (game->player, output) &&
	writestring (game->levelpackfile, output) &&
	writeint (&game->state, output) &&
	writeint (&game->levelid, output) &&
	writeint (&game->turnno, output);

    /* save the level progress */
    r = r && game->level->write (game->level, output);

    /* save the library */
    for (c = 0; c < 12; ++c)
	r = r && writeint (&game->library[c]->type, output);

    /* save the score */
    strcpy (game->score->player, game->player); /* again */
    r = r && game->score->write (game->score, output);

    /* close the output file and return */
    fclose (output);
    return r;
}

/**
 * Load a game. The filename is taken from the attributes.
 * @param  game      The game to load.
 * @param  summary 0 to load the full game, 1 for summary only.
 * @return         1 if successful, 0 on failure.
 */
static int load (Game *game, int summary)
{
    FILE *input; /* the input file */
    int r = 1, /* return value */
	c, /* general counter */
	type; /* type read from file */
    char header[8]; /* header read from file */

    /* open the input file */
    if (! (input = fopen (game->filename, "rb")))
	return 0;

    /* read the game header */
    r = r &&
	fread (header, 8, 1, input) &&
	! strncmp (header, "TDR100G", 8);

    /* read the game settings */
    r = r && readstring (game->player, input);
    r = r && readstring (game->levelpackfile, input);

    /* stop here if only the summary is needed */
    if (summary) {
	fclose (input);
	return r;
    }

    /* load the game state and progress information */
    r = r && readint (&game->state, input);
    r = r && readint (&game->levelid, input);
    r = r && readint (&game->turnno, input);

    /* load in the level pack and current level state */
    if (r) {
	if (game->levelpack)
	    game->levelpack->clear (game->levelpack);
	else if (! (game->levelpack = new_LevelPack ()))
	    fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
	strcpy (game->levelpack->filename, game->levelpackfile);
	r = r && game->levelpack->load (game->levelpack, 0);
	if (game->level)
	    game->level->destroy (game->level);
	game->level = new_Level ();
	r = r &&
	    game->level->read (game->level, input);
    }

    /* load the library */
    for (c = 0; c < 12; ++c)
	if ((r = r && readint (&type, input)))
	    game->library[c] = get_Action (type);

    /* load the score */
    r = r && game->score->read (game->score, input);

    /* close the input file and return */
    fclose (input);
    return r;
}

/**
 * Shuffle the actions at the start of a level.
 * @param  game The game whose actions are to be shuffled.
 */
static void shuffleactions (Game *game)
{
    int c, /* general counter */
	s; /* action to swap with */
    Action *temp; /* temporary action for swapping */
    srand (time (NULL));
    for (c = 0; c < 12; ++c) {
	s = rand () % 12;
	temp = game->library[c];
	game->library[c] = game->library[s];
	game->library[s] = temp;
    }
}

/**
 * End the current turn.
 * @param  game The game to advance.
 */
static int turn (Game *game)
{
    Action *temp; /* temporary action for rotation */
    int c, /* general counter */
	r; /* RAM counter */
    Robot *robot; /* pointer to robot */
    Level *level; /* new level pointer */

    /* advance turn */
    ++game->turnno;

    /* rotate library */
    temp = game->library[0];
    for (c = 0; c < 11; ++c)
	game->library[c] = game->library[c + 1];
    game->library[11] = temp;

    /* clear robots' ram */
    for (c = 0; c < 192; ++c)
	if ((robot = game->level->robots[c]) &&
	    robot->type != ROBOT_GUARD)
	    for (r = 0; r < robot->ramsize; ++r)
		robot->ram[r] = 0;

    /* check for level complete */
    if (checkcomplete (game)) {
	game->score->scores[game->levelid] = game->turnno;
	strcpy (game->score->player, game->player); /* again */
	if (game->levelid == 11)
	    return game->state = STATE_VICTORY;
	else {
	    game->turnno = 0;
	    ++game->levelid;
	    game->level->destroy (game->level);
	    level = game->levelpack->levels[game->levelid];
	    game->level = level->clone (level);
	    return game->state = STATE_COMPLETE;
	}
    }

    /* check for level failed */
    if (checkfailed (game)) {
	game->level->destroy (game->level);
	level = game->levelpack->levels[game->levelid];
	game->level = level->clone (level);
	return game->state = STATE_FAILED;
    }
	
    /* set the appropriate state */
    return game->state = STATE_PROGRAM;
}

/*----------------------------------------------------------------------
 * Constructor Level Function Definitions.
 */

/**
 * Game contstructor function.
 * @return The new game.
 */
Game *new_Game (void)
{
    Game *game; /* the new game */

    /* reserve memory for the game and reports */
    if (! (game = malloc (sizeof (Game))))
	return NULL;

    /* initialise the methods */
    game->destroy = destroy;
    game->clear = clear;
    game->save = save;
    game->load = load;
    game->shuffleactions = shuffleactions;
    game->turn = turn;

    /* grab library pointers before we use them */
    config = getconfig ();

    /* initialise the attributes */
    createobjects (game);
    initialiseattributes (game);

    /* return the new game */
    return game;
}
