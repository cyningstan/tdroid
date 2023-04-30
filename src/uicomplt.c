/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Level Complete Screen Module.
 */

/*----------------------------------------------------------------------
 * Includes.
 */

/* ANSI C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* project-specific headers */
#include "uiscreen.h"
#include "tdroid.h"
#include "display.h"
#include "controls.h"
#include "config.h"
#include "game.h"
#include "scoretbl.h"
#include "fatal.h"

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/**
 * @struct UIScreenData
 * Data required for the Score screen.
 */
struct uiscreendata {

    /** @var game A pointer to the game. */
    Game *game;

    /** @var scoretable A prospective score table. */
    ScoreTable *scoretable;

    /** @var rank The player's prospective rank. */
    int rank;

};

/** @var display A pointer to the display module. */
static Display *display;

/** @var controls A pointer to the game controls module. */
static Controls *controls;

/** @var config A pointer to the configuration. */
static Config *config;

/** @var newgamemenu The menu for a new game. */
static char *scoremenu[] = {
    "Cancel menu",
    "Proceed",
    "New game",
    "Exit game"
};

/*----------------------------------------------------------------------
 * Public Method Function Declarations.
 */

/**
 * Destroy the Level Complete screen.
 * @param uiscreen The screen to destroy.
 */
static void destroy (UIScreen *uiscreen)
{
    if (uiscreen) {
	if (uiscreen->data) {
	    if (uiscreen->data->scoretable)
		uiscreen->data->scoretable->destroy
		    (uiscreen->data->scoretable);
	    free (uiscreen->data);
	}
	free (uiscreen);
    }
}

/**
 * Initialise the UI state when first encountered.
 * @param uiscreen The screen to affect.
 */
static void init (UIScreen *uiscreen)
{
    Game *game; /* pointer to current game */
    Score *score; /* pointer to current scores */
    ScoreTable *scoretable; /* pointer to the prospective scoretable */
    int rank = -1; /* rank of player's score */

    /* intialise convenience variables */
    game = uiscreen->data->game;
    score = game->score;
    strcpy (score->player, game->player); /* AGAIN */

    /* create a cloned score table and insert the scores into it */
    scoretable = game->levelpack->scoretable->clone
	(game->levelpack->scoretable);
    scoretable->sort (scoretable, game->levelid);
    rank = scoretable->insert (scoretable, score, game->levelid - 1);

    /* store relevant details in the screen data */
    uiscreen->data->scoretable = scoretable;
    uiscreen->data->rank = rank;
}

/**
 * Show the UI screen.
 * @param uiscreen The screen to show.
 * @return         The ID of the screen to show next.
 */
static UIState show (UIScreen *uiscreen)
{
    int option; /* option chosen from the menu */

    /* initialise the display */
    display->showscorescreen
	("LEVEL", "COMPLETE",
	 uiscreen->data->game->levelpack->name,
	 uiscreen->data->scoretable,
	 uiscreen->data->game->levelid,
	 uiscreen->data->rank);
    display->update ();

    /* main loop */
    while (1) {

	/* wait for fire to be pressed */
	while (! controls->fire ());

	/* get a choice from the menu */
	option = display->menu (4, scoremenu, 1);
	switch (option) {

	case 0: /* cancel menu */
	    break;

	case 1: /* proceed */
	    return (uiscreen->data->game->state = STATE_DEPLOY);

	case 2: /* new game */
	    return STATE_NEWGAME;

	case 3: /* exit game */
	    return STATE_QUIT;
	}
    }
}

/*----------------------------------------------------------------------
 * Top Level Function Declarations.
 */

/**
 * Construct the Best Scores Screen.
 * @param  game The game object.
 * @return      The new New Game Screen.
 */
UIScreen *new_LevelCompleteScreen (Game *game)
{
    UIScreen *uiscreen; /* the screen */

    /* Reserve memory for the UI screen object */
    if (! (uiscreen = new_UIScreen ()))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
    if (! (uiscreen->data = malloc (sizeof (UIScreenData))))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* get pointers to necessary modules */
    display = getdisplay ();
    controls = getcontrols ();
    config = getconfig ();

    /* initialise the methods */
    uiscreen->destroy = destroy;
    uiscreen->init = init;
    uiscreen->show = show;

    /* initialise attributes */
    uiscreen->data->game = game;
    uiscreen->data->scoretable = NULL;
    uiscreen->data->rank = -1;

    /* return the new user interface screen */
    return uiscreen;
}
