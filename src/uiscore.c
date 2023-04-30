/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Score Screen Module.
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
 * Initialise the UI state when first encountered.
 * @param uiscreen The screen to affect.
 */
static void init (UIScreen *uiscreen)
{
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
	("BEST", "SCORES",
	 uiscreen->data->game->levelpack->name,
	 uiscreen->data->game->levelpack->scoretable, 12, -1);
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
	    break;

	case 2: /* new game */
	    return STATE_NEWGAME;
	    break;

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
UIScreen *new_ScoreScreen (Game *game)
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
    uiscreen->init = init;
    uiscreen->show = show;

    /* initialise attributes */
    uiscreen->data->game = game;

    /* return the new user interface screen */
    return uiscreen;
}
