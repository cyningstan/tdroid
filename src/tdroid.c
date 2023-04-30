/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Main Program.
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
#include "controls.h"
#include "display.h"
#include "fatal.h"
#include "config.h"
#include "game.h"
#include "robot.h"
#include "uiscreen.h"

/*----------------------------------------------------------------------
 * Data Definitions
 */

/**
 * @var colourset is the colour set:
 * 0 for 640x200 black and white dithered;
 * 1 for 320x200 black, light cyan, magenta, white;
 * 2 for 320x200 dark cyan, light cyan, red, white.
 */
static int colourset = 2;

/** @var quiet Game is silent if 1, or has sound and music if 0. */
static int quiet = 0;

/** @var controls The controls object. */
static Controls *controls = NULL;

/** @var display is the display object. */
static Display *display = NULL;

/** @var game is a loaded game. */
static Game *game = NULL;

/** @var config Pointer to the configuration. */
static Config *config = NULL;

/** @var state The current unsaved game state. */
static int state;

/** @var playerrobots The six player robots. */
static Robot *playerrobots[6];

/** @var guardrobot The guard robot. */
static Robot *guardrobot;

/*----------------------------------------------------------------------
 * Level 2 Routines
 */

/**
 * Read through the argument list for recognised parameters.
 * @param argc is the argument count from the command line.
 * @param argv is an array of arguments.
 */
static void initialiseargs (int argc, char **argv)
{
    /* local variables */
    int c; /* argument count */

    /* scan through arguments */
    for (c = 1; c < argc; ++c)
	if (! strcmp (argv[c], "-p"))
	    colourset = 1;
	else if (! strcmp (argv[c], "-m"))
	    colourset = 0;
	else if (! strcmp (argv[c], "-q"))
	    quiet = 1;
	else
	    fatalerror (FATAL_COMMAND_LINE, __FILE__, __LINE__);
}

/*----------------------------------------------------------------------
 * Level 1 Routines
 */

/**
 * Initialise the program.
 * @param argc is the argument count from the command line.
 * @param argv is the array of arguments from the command line.
 */
static void initialiseprogram (int argc, char **argv)
{
    /* check command line */
    initialiseargs (argc, argv);

    /* initialise the random number generator */
    srand (time (NULL));

    /* initialise controls */
    if (! (controls = new_Controls ()))
	fatalerror (FATAL_DISPLAY, __FILE__, __LINE__);

    /* initialise the display and assets */
    if (! (display = new_Display (colourset, quiet)))
	fatalerror (FATAL_DISPLAY, __FILE__, __LINE__);
    display->showtitlescreen ();

    /* initialise configuration */
    config = new_Config ();
    config->load ();

    /* initialise the game object */
    game = new_Game ();
    if (*config->gamefile) {
	strcpy (game->filename, config->gamefile);
	if (game->load (game, 0))
	    display->setgame (game);
	else {
	    game->clear (game);
	    *config->gamefile = '\0';
	}
    }
    state = game->state;

    /* await the fire key */
    display->titlekey ();
}

/**
 * The main game loop selects a screen to activate.
 */
static int playgame (void)
{
    UIScreen *uiscreen; /* user interface screen to present */

    /* build the screen */
    switch (state) {
    case STATE_NEWGAME:
	uiscreen = new_NewGameScreen (game);
	break;
    case STATE_SCORE:
	uiscreen = new_ScoreScreen (game);
	break;
    case STATE_DEPLOY:
	uiscreen = new_DeploymentScreen (game);
	break;
    case STATE_PROGRAM:
	uiscreen = new_ProgrammingScreen (game);
	break;
    case STATE_ACTION:
	uiscreen = new_ActionScreen (game);
	break;
    case STATE_COMPLETE:
	uiscreen = new_LevelCompleteScreen (game);
	break;
    case STATE_VICTORY:
	uiscreen = new_VictoryScreen (game);
	break;
    default:
	return 0;	
    }

    /* show the screen */
    uiscreen->init (uiscreen);
    state = uiscreen->show (uiscreen);
    uiscreen->destroy (uiscreen);

    /* return */
    return (state != STATE_QUIT);
}

/**
 * Clean up when the user quits normally.
 */
static void endprogram (void)
{
    /* save the configuration and current game */
    config->save ();
    if (*game->filename) {
	game->save (game);
	game->destroy (game);
    }

    /* destroy global objects */
    display->destroy ();
    controls->destroy ();
    config->destroy ();
    destroy_Cells ();
    destroy_Actions ();
}

/**
 * Share the control handler with other modules.
 * @return A pointer to the game control handler.
 */
Controls *getcontrols (void)
{
    return controls;
}

/**
 * Share the display handler with other modules.
 * @return A pointer to the display module.
 */
Display *getdisplay (void)
{
    return display;
}

/**
 * Share the config handler with other modules.
 * @return A pointer to the config module.
 */
Config *getconfig (void)
{
    return config;
}

/**
 * Load the robots from the already-open asset file.
 * @param input The asset file handle.
 */
void loadrobotdetails (FILE *input)
{
    int c; /* robot counter */

    /* read the player robots */
    for (c = 0; c < 6; ++c)
	if (! (playerrobots[c] = new_Robot (c + 1)))
	    fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
	else if (! playerrobots[c]->read (playerrobots[c], input))
	    fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);

    /* read the guard robot */
    if (! (guardrobot = new_Robot (7)))
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    else if (! guardrobot->read (guardrobot, input))
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
}

/**
 * Create a new player robot and populate its details.
 * @param  type The type of robot.
 * @return      The player robot.
 */
Robot *new_PlayerRobot (int type)
{
    return playerrobots[type - 1]->clone (playerrobots[type - 1]);
}

/**
 * Main Program.
 * @param argc is the number of command line arguments.
 * @param argv is an array of command line arguments.
 * @return 0 if successful, >0 on error.
 */
int main (int argc, char **argv)
{
    initialiseprogram (argc, argv);
    while (playgame ());
    endprogram ();
    return 0;
}
