/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Programming User Interface Screen Module.
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
#include "action.h"
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

    /**
     * @var cursor
     * Current cursor location.
     * -8..-15 in the RAM.
     * -2..-7  in the library.
     * -1      in the ROM.
     * 0..191  on the map.
     */
    int cursor;

    /** @var current The current robot to program. */
    Robot *current;

};

/** @var display A pointer to the display module. */
static Display *display;

/** @var controls A pointer to the game controls module. */
static Controls *controls;

/** @var config A pointer to the configuration. */
static Config *config;

/** @var programmenu The menu for the programming screen. */
static char *programmenu[] = {
    "Cancel menu",
    "Select robot",
    "Add action",
    "Move later",
    "Remove action",
    "Move earlier",
    "Go!",
    "Reset level",
    "New game",
    "Exit game"
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
    int ramsize; /* ram size of the current robot */

    /* get a RAM size for the current robot */
    if (uiscreen->data->current)
	ramsize = uiscreen->data->current->ramsize;
    else
	ramsize = 0;

    /* main navigation loop */
    do {
	if (uiscreen->data->cursor <= -8)
	    uiscreen->data->cursor =
		display->navigateram (uiscreen->data->cursor,
				      ramsize);
	else if (uiscreen->data->cursor < -1)
	    uiscreen->data->cursor =
		display->navigatelibrary (uiscreen->data->cursor);
	else if (uiscreen->data->cursor == -1)
	    uiscreen->data->cursor =
		display->navigaterom (uiscreen->data->cursor);
	else
	    uiscreen->data->cursor =
		display->navigateprogrammap (uiscreen->data->cursor);
    } while (! controls->fire ());
}

/**
 * Work out the default menu option for the current situation.
 * @param  uiscreen The user interface screen.
 * @return          The default option.
 */
static int getdefaultoption (UIScreen *uiscreen)
{
    int cursor, /* current cursor position */
	ramslot; /* the RAM slot selected */
    Robot *current; /* pointer to current robot */
    Level *level; /* pointer to current level */

    /* initialise convenience variables */
    cursor = uiscreen->data->cursor;
    current = uiscreen->data->current;
    level = uiscreen->data->game->level;
    ramslot = -cursor - 8;

    /* if the cursor is on a robot on the map, "Select" */
    if (cursor >= 0 &&
	level->robots[cursor] &&
	level->robots[cursor]->type != ROBOT_GUARD)
	return 1;

    /* if the cursor is elsewhere on the map, "Cancel" */
    if (cursor >= 0)
	return 0;

    /* if robot selected and the cursor is in the libray, "Add" */
    if (current && cursor >= -7 && cursor <= -1)
	return 2;

    /* if robot selected and cursor is in ROM, "Add" */
    if (cursor == -1 && current && current->rom)
	return 2;

    /* if robot selected and cursor is on an action in RAM, "Remove" */
    if (current &&
	cursor >= -15 && cursor <= -8 &&
	current->ram[ramslot])
	return 4;

    /* otherwise "Cancel" */
    return 0;
}

/**
 * Select a robot on the map.
 * @param uiscreen The user interface screen.
 */
static void selectrobot (UIScreen *uiscreen)
{
    Robot *current; /* current robot */
    Game *game; /* a pointer to the game */
    int cursor; /* the cursor location */

    /* initialise convenience variables */
    game = uiscreen->data->game;
    cursor = uiscreen->data->cursor;

    /* see if there's anything under the cursor */
    if (cursor >= 0 && game->level->robots[cursor])
	current = game->level->robots[cursor];
    else {
	uiscreen->inform ("There is no robot here!");
	return;
    }

    /* make sure it's not a guard */
    if (current->type == ROBOT_GUARD) {
	uiscreen->inform ("You cannot program guard robots!");
	return;
    }

    /* display the robot in the "current droid" window */
    display->showrobottoprogram (current, uiscreen->data->game->level);
    display->update ();
    uiscreen->data->current = current;
}

/**
 * Add an action to the robot's RAM.
 * @param uiscreen The user interface screen.
 */
static void addaction (UIScreen *uiscreen)
{
    Robot *robot; /* pointer to the robot */
    Level *level; /* pointer to the level */
    Action **library; /* pointer to the library */
    int cursor, /* cursor position */
	c, /* general counter */
	next = 0, /* next blank instruction slot */
	libslot; /* library slot pointed to */

    /* initialise convenience variables */
    cursor = uiscreen->data->cursor;
    robot = uiscreen->data->current;
    level = uiscreen->data->game->level;
    library = uiscreen->data->game->library;
    libslot = -cursor - 2;

    /* check we have a robot selected */
    if (! robot) {
	uiscreen->inform ("Select a robot first!");
	return;
    } else if (cursor < -7 || cursor > -1 ||
	       (cursor == -1 && ! robot->rom)) {
	uiscreen->inform ("Point to an action first!");
	return;
    }

    /* see where the action will be added */
    for (c = 0; c < robot->ramsize; ++c)
	if (robot->ram[c])
	    next = c + 1;
    if (next >= robot->ramsize) {
	uiscreen->inform ("The RAM is full!");
	return;
    }

    /* put the action into the RAM */
    robot->ram[next] = (cursor == -1)
	? robot->rom :
	library[libslot]->type;
    display->showrobottoprogram (robot, level);
    display->update ();
}

/**
 * Remove an action from the robot's RAM.
 * @param uiscreen The user interface screen.
 */
static void removeaction (UIScreen *uiscreen)
{
    Robot *robot; /* pointer to the robot */
    Level *level; /* pointer to the level */
    int cursor, /* cursor position */
	ramslot; /* RAM slot pointed to */

    /* initialise convenience variables */
    cursor = uiscreen->data->cursor;
    robot = uiscreen->data->current;
    level = uiscreen->data->game->level;
    ramslot = -cursor - 8;

    /* validate robot and action selection */
    if (! robot) {
	uiscreen->inform ("Select a robot first!");
	return;
    } else if (cursor < -15 || cursor > -8 ||
	       ! robot->ram[ramslot]) {
	uiscreen->inform ("Point to an action in RAM first!");
	return;
    }

    /* remove the action from the RAM */
    robot->ram[ramslot] = ACTION_NONE;
    display->showrobottoprogram (robot, level);
    display->update ();
}

/**
 * Move an action earlier in the robot's RAM.
 * @param uiscreen The user interface screen.
 */
static void moveactionearlier (UIScreen *uiscreen)
{
    Robot *robot; /* pointer to the robot */
    Level *level; /* pointer to the level */
    int cursor, /* cursor position */
	ramslot, /* RAM slot pointed to */
	temp; /* temporary swap variable */

    /* initialise convenience variables */
    cursor = uiscreen->data->cursor;
    robot = uiscreen->data->current;
    level = uiscreen->data->game->level;
    ramslot = -cursor - 8;

    /* validate robot and action selection */
    if (! robot) {
	uiscreen->inform ("Select a robot first!");
	return;
    } else if (cursor < -15 || cursor > -8 ||
	       ramslot >= robot->ramsize) {
	uiscreen->inform ("Point to a slot in RAM first!");
	return;
    } else if (cursor == -8) {
	uiscreen->inform ("This is the first action slot!");
	return;
    }

    /* swap this action with the later one in RAM */
    temp = robot->ram[ramslot];
    robot->ram[ramslot] = robot->ram[ramslot - 1];
    robot->ram[ramslot - 1] = temp;
    ++uiscreen->data->cursor;

    /* update the display */
    display->showrobottoprogram (robot, level);
    display->update ();
}

/**
 * Move an action later in the robot's RAM.
 * @param uiscreen The user interface screen.
 */
static void moveactionlater (UIScreen *uiscreen)
{
    Robot *robot; /* pointer to the robot */
    Level *level; /* pointer to the level */
    int cursor, /* cursor position */
	ramslot, /* RAM slot pointed to */
	temp; /* temporary swap variable */

    /* initialise convenience variables */
    cursor = uiscreen->data->cursor;
    robot = uiscreen->data->current;
    level = uiscreen->data->game->level;
    ramslot = -cursor - 8;

    /* validate robot and action selection */
    if (! robot) {
	uiscreen->inform ("Select a robot first!");
	return;
    } else if (cursor < -15 || cursor > -8 ||
	       ramslot >= robot->ramsize) {
	uiscreen->inform ("Point to a slot in RAM first!");
	return;
    } else if (ramslot == robot->ramsize - 1) {
	uiscreen->inform ("This is the last action slot!");
	return;
    }

    /* swap this action with the later one in RAM */
    temp = robot->ram[ramslot];
    robot->ram[ramslot] = robot->ram[ramslot + 1];
    robot->ram[ramslot + 1] = temp;
    --uiscreen->data->cursor;

    /* update the display */
    display->showrobottoprogram (robot, level);
    display->update ();
}

/**
 * Reset the level to its initial state.
 * @param uiscreen The user interface screen.
 */
static void resetlevel (UIScreen *uiscreen)
{
    Game *game; /* pointer to game */
    Level *initial; /* initial state of the current level */
    game = uiscreen->data->game;
    initial = game->levelpack->levels[game->levelid];
    game->level->destroy (game->level);
    game->level = initial->clone (initial);
    game->turnno = 0;
    game->state = STATE_DEPLOY;
}

/*----------------------------------------------------------------------
 * Public Method Function Definitions.
 */

/**
 * Initialise the UI state when first encountered.
 * @param uiscreen The screen to affect.
 */
static void init (UIScreen *uiscreen)
{
    int c; /* general counter */
    Level *level; /* pointer to current level state */
    level = uiscreen->data->game->level;
    uiscreen->data->cursor = -7;
    for (c = 0; c < 192 && ! uiscreen->data->current; ++c)
	if (level->robots[c] && level->robots[c]->type != ROBOT_GUARD)
	    uiscreen->data->current = level->robots[c];
}

/**
 * Show the UI screen.
 * @param uiscreen The screen to show.
 * @return         The ID of the screen to show next.
 */
static UIState show (UIScreen *uiscreen)
{
    int option; /* an option chosen from the menu */

    /* show the initial display */
    display->showprogrammingscreen (uiscreen->data->game->library,
				    uiscreen->data->current,
				    uiscreen->data->game->level);
    display->update ();

    /* main loop */
    while (1) {

	/* allow navigation of the cursor */
	navigatecursor (uiscreen);

	/* get a choice from the menu */
	option = getdefaultoption (uiscreen);
	option = display->menu (10, programmenu, option);

	switch (option) {

	case 0: /* cancel menu */
	    break;

	case 1: /* select robot */
	    selectrobot (uiscreen);
	    break;

	case 2: /* add action */
	    addaction (uiscreen);
	    break;

	case 3: /* move action later */
	    moveactionlater (uiscreen);
	    break;

	case 4: /* remove action */
	    removeaction (uiscreen);
	    break;

	case 5: /* move action earlier */
	    moveactionearlier (uiscreen);
	    break;

	case 6: /* go! */
	    return STATE_ACTION;
	    break;

	case 7: /* reset level */
	    if (uiscreen->confirm ("Reset this level and try again?")) {
		resetlevel (uiscreen);
		return uiscreen->data->game->state;
	    }
	    break;

	case 8: /* new game */
	    return STATE_NEWGAME;
	    break;

	case 9: /* exit game */
	    return STATE_QUIT;
	}
    }
}

/*----------------------------------------------------------------------
 * Top Level Function Declarations.
 */

/**
 * Construct the Programming Screen.
 * @param  game The game object.
 * @return      The new Programming Screen.
 */
UIScreen *new_ProgrammingScreen (Game *game)
{
	UIScreen *uiscreen; /* screen to return */

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
	uiscreen->init = init;
	uiscreen->show = show;

	/* initialise attributes */
	uiscreen->data->game = game;
	uiscreen->data->cursor = 0;
	uiscreen->data->current = NULL;

	/* return the screen */
	return uiscreen;
}

