/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Deployment User Interface Screen Module.
 */

/*----------------------------------------------------------------------
 * Required Headers.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* project specific headers */
#include "uiscreen.h"
#include "tdroid.h"
#include "display.h"
#include "controls.h"
#include "config.h"
#include "game.h"
#include "level.h"
#include "robot.h"
#include "item.h"
#include "fatal.h"


/*----------------------------------------------------------------------
 * Data Definitions.
 */

/**
 * @struct uiscreendata
 * Data for the deployment screen.
 */
struct uiscreendata {

    /** @var game The game data. */
    Game *game;

    /** @var robots The robots on the panel. */
    Robot *robots[6];

    /**
     * @var cursor
     * Current cursor location.
     * -1..-6  on the team panel.
     * 0..191  on the map.
     */
    int cursor;

    /** @var current The current robot to deploy. */
    Robot *current;

};

/** @var display A pointer to the display module. */
static Display *display;

/** @var controls A pointer to the game controls module. */
static Controls *controls;

/** @var config A pointer to the configuration. */
static Config *config;

/** @var deploymentmenu The menu for the deployment screen. */
static char *deploymentmenu[] = {
    "Cancel menu",
    "Select robot",
    "Deploy robot",
    "Remove robot",
    "Proceed",
    "New Game",
    "Exit Game"
};

/*----------------------------------------------------------------------
 * Level 1 Function Definitions.
 */

/**
 * Navigate the cursor around the panel and map.
 * @param uiscreen The user interface screen.
 */
static void navigatecursor (UIScreen *uiscreen)
{
    do {
	if (uiscreen->data->cursor < 0)
	    uiscreen->data->cursor =
		display->navigatedeploypanel (uiscreen->data->cursor);
	else
	    uiscreen->data->cursor =
		display->navigatedeploymap (uiscreen->data->cursor);
    } while (! controls->fire ());
}

/**
 * Work out the default menu option for the current situation.
 * @param  uiscreen The user interface screen.
 * @return          The default option.
 */
static int getdefaultoption (UIScreen *uiscreen)
{
    Robot *current, /* current robot */
	**robots; /* the robots array */
    Level *level; /* a pointer to the level */
    int cursor; /* the cursor location */

    /* initialise convenience variables */
    robots = uiscreen->data->robots;
    level = uiscreen->data->game->level;
    current = uiscreen->data->current;
    cursor = uiscreen->data->cursor;

    /* is a robot under the cursor? */
    if (cursor < 0 && robots[-cursor - 1])
	return 1; /* select robot */

    /* is reserved robot selected, and cursor on item spawner? */
    else if (current &&
	     robots[current->type - 1] &&
	     cursor >= 0 &&
	     level->items[cursor] &&
	     level->items[cursor]->type == ITEM_SPAWNER)
	return 2; /* deploy */

    /* is deployed robot selected? */
    else if (cursor >= 0 &&
	     level->robots[cursor] &&
	     level->robots[cursor]->type != ROBOT_GUARD)
	return 3; /* remove */
	
    return 0;
}

/**
 * Attempt to select a robot at the cursor location.
 * @param uiscreen The user interface screen.
 */
static void selectrobot (UIScreen *uiscreen)
{
    Robot *current, /* current robot */
	**robots; /* the robots array */
    Game *game; /* a pointer to the game */
    int cursor; /* the cursor location */

    /* initialise convenience variables */
    robots = uiscreen->data->robots;
    game = uiscreen->data->game;
    cursor = uiscreen->data->cursor;

    /* see if there's anything under the cursor */
    if (cursor < 0 && robots[-cursor - 1])
	current = robots[-cursor - 1];
    else if (cursor >= 0 && game->level->robots[cursor])
	current = game->level->robots[cursor];
    else {
	uiscreen->inform ("There is no robot here!");
	return;
    }

    /* make sure it's not a guard */
    if (current->type == ROBOT_GUARD) {
	uiscreen->inform ("You cannot deploy guard robots!");
	return;
    }

    /* display the robot in the "current droid" window */
    display->showrobottodeploy (current);
    display->update ();
    uiscreen->data->current = current;
}

/**
 * Attempt to deploy a robot at the cursor location.
 * @param uiscreen The user interface screen.
 */
static void deployrobot (UIScreen *uiscreen)
{
    Robot *current, /* current robot */
	**robots; /* the robots array */
    Level *level; /* a pointer to the level */
    int cursor, /* the cursor location */
	x, /* x cursor location */
	y; /* y cursor location */

    /* initialise convenience variables */
    robots = uiscreen->data->robots;
    level = uiscreen->data->game->level;
    current = uiscreen->data->current;
    cursor = uiscreen->data->cursor;

    /* validate deployment */
    if (! current)
	uiscreen->inform ("Select a robot first!");
    else if (cursor < 0)
	uiscreen->inform ("Point at a map square to deploy!");
    else if (! level->items[cursor] ||
	     level->items[cursor]->type != ITEM_SPAWNER)
	uiscreen->inform ("You can only deploy at a spawner!");
    else if (! robots[current->type - 1])
	uiscreen->inform ("That robot is already deployed!");

    /* deploy the robot */
    else {

	/* ascertain the robot facing */
	x = cursor % 16;
	y = cursor / 16;
	if (x < 2 + y && x < 13 - y)
	    current->facing = ROBOT_EAST;
	else if (x > 2 + y && x > 13 - y)
	    current->facing = ROBOT_WEST;
	else if (y < 6)
	    current->facing = ROBOT_SOUTH;
	else
	    current->facing = ROBOT_NORTH;

	/* update the robot data */
	current->x = x;
	current->y = y;
	    
	/* update the level and panel data */
	level->robots[cursor] = current;
	level->items[cursor]->destroy (level->items[cursor]);
	level->items[cursor] = NULL;
	robots[current->type - 1] = NULL;

	/* update the display */
	display->showdeploymentrobot (current->type, 0);
	display->showrobottodeploy (current);
	display->showlevelmapsquare (level, cursor);
	display->update ();
	display->playsound (DISPLAY_NOISE_DEPLOY);

    }
}

/**
 * Attempt to remove a robot from the cursor location.
 * @param uiscreen The user interface screen.
 */
static void removerobot (UIScreen *uiscreen)
{
    Robot *robot, /* robot pointed to */
	**robots; /* the robots array */
    Level *level; /* a pointer to the level */
    int cursor; /* the cursor location */

    /* initialise convenience variables */
    robots = uiscreen->data->robots;
    level = uiscreen->data->game->level;
    cursor = uiscreen->data->cursor;

    /* validate removal */
    if (cursor < 0)
	uiscreen->inform ("Point to a square on the map first");
    else if (! (robot = level->robots[cursor]))
	uiscreen->inform ("There is no robot here!");
    else if (robot->type == ROBOT_GUARD)
	uiscreen->inform ("Nice try! The guard stays put");

    /* remove the robot */
    else {

	/* update the robot data */
	robot->x = 0xff;
	robot->y = 0xff;
	    
	/* put the robot back in the panel */
	robots[robot->type - 1] = robot;
	level->robots[cursor] = NULL;
	level->items[cursor] = new_Item (ITEM_SPAWNER);

	/* update the display */
	display->showdeploymentrobot (robot->type, 1);
	display->showlevelmapsquare (level, cursor);
	display->update ();

    }
}

/**
 * Check to see if any robots have been deployed.
 * @param uiscreen The user interface screen.
 */
static int checkrobotdeployed (UIScreen *uiscreen)
{
    int c; /* robot counter */
    for (c = 0; c < 6; ++c)
	if (! uiscreen->data->robots[c])
	    return 1;
    uiscreen->inform ("You've not deployed any robots!");
    return 0;
}

/**
 * Remove any unused spawners.
 * @param level The level to play.
 */
static void clearspawners (Level *level)
{
    int c; /* cell counter */
    for (c = 0; c < 192; ++c)
	if (level->items[c] && level->items[c]->type == ITEM_SPAWNER) {
	    level->items[c]->destroy (level->items[c]);
	    level->items[c] = NULL;
	}
}

/*----------------------------------------------------------------------
 * Public Method Function Definitions.
 */

/**
 * Destroy the current UI screen when no longer needed.
 * @param uiscreen The screen to destroy.
 */
static void destroy (UIScreen *uiscreen)
{
    Robot *robot; /* pointer to a robot */
    int c; /* general counter */
    if (uiscreen) {
	if (uiscreen->data) {
	    for (c = 0; c < 6; ++c)
		if ((robot = uiscreen->data->robots[c]))
		    robot->destroy (robot);
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
    Level *level; /* a pointer to the current level */
    Robot **robots, /* a pointer to the panel robots */
	*robot; /* pointer to a robot on the map */
    int c; /* general counter */

    /* set up convenience variables */
    level = uiscreen->data->game->level;
    robots = uiscreen->data->robots;

    /* assume first that all robots are on the panel */
    for (c = 0; c < 6; ++c)
	robots[c] = new_PlayerRobot (c + 1);

    /* scan the map to see which robots have been deployed */
    for (c = 0; c < 192; ++c)
	if ((robot = level->robots[c]) &&
	    robot->type > ROBOT_NONE &&
	    robot->type < ROBOT_GUARD) {
	    robots[robot->type - 1]->destroy (robots[robot->type - 1]);
	    robots[robot->type - 1] = NULL;
	}
}

/**
 * Show the UI screen.
 * @param uiscreen The screen to show.
 * @return         The ID of the screen to show next.
 */
static UIState show (UIScreen *uiscreen)
{
    int option; /* option chosen from menu */

    /* show the initial display */
    display->showdeploymentscreen (uiscreen->data->robots,
				   uiscreen->data->game->level);
    display->update ();

    /* main loop */
    while (1) {

	/* allow navigation of the cursor */
	navigatecursor (uiscreen);

	/* get a choice from the menu */
	option = getdefaultoption (uiscreen);
	option = display->menu (7, deploymentmenu, option);
	switch (option) {

	case 0: /* cancel menu */
	    break;

	case 1: /* select robot */
	    selectrobot (uiscreen);
	    break;

	case 2: /* deploy robot */
	    deployrobot (uiscreen);
	    break;

	case 3: /* remove robot */
	    removerobot (uiscreen);
	    break;

	case 4: /* proceed */
	    if (checkrobotdeployed (uiscreen)) {
		clearspawners (uiscreen->data->game->level);
		uiscreen->data->game->state = STATE_PROGRAM;
		uiscreen->data->game->shuffleactions (uiscreen->data->game);
		return STATE_PROGRAM;
	    }
	    break;

	case 5: /* new game */
	    return STATE_NEWGAME;
	    break;

	case 6: /* exit game */
	    return STATE_QUIT;
	}
    }
}

/*----------------------------------------------------------------------
 * Top Level Function Definitions.
 */

/**
 * Construct the Deployment Screen.
 * @param  game The game object.
 * @return      The new Deployment Screen.
 */
UIScreen *new_DeploymentScreen (Game *game)
{
    UIScreen *uiscreen; /* screen to return */
    int c; /* general counter */

    /* reserve memory for UI screen */
    if (! (uiscreen = new_UIScreen ()))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
    if (! (uiscreen->data = malloc (sizeof (UIScreenData))))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* get pointers to necessary modules */
    display = getdisplay ();
    controls = getcontrols ();
    config = getconfig ();

    /* initialise methods */
    uiscreen->destroy = destroy;
    uiscreen->init = init;
    uiscreen->show = show;

    /* initialise attributes */
    uiscreen->data->game = game;
    for (c = 0; c < 6; ++c)
	uiscreen->data->robots[c] = NULL;
    uiscreen->data->cursor = -1;
    uiscreen->data->current = NULL;

    /* return the screen */
    return uiscreen;
}

