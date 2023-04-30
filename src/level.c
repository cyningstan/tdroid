/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker 2022.
 * Level module.
 */

/*----------------------------------------------------------------------
 * Required Headers.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* project specific headers */
#include "level.h"
#include "fatal.h"
#include "cell.h"
#include "robot.h"
#include "item.h"
#include "utils.h"


/*----------------------------------------------------------------------
 * Data Definitions.
 */

/*----------------------------------------------------------------------
 * Public Method Function Definitions.
 */

/**
 * Destroy the level when it is no longer needed.
 * @param level The level to destroy.
 */
static void destroy (Level *level)
{
    int c; /* general counter */
    Robot *robot; /* pointer to a robot */
    Item *item; /* pointer to an item */
    if (level) {
	for (c = 0; c < 192; ++c) {
	    if ((item = level->items[c]))
		item->destroy (item);
	    if ((robot = level->robots[c]))
		robot->destroy (robot);
	}
	free (level);
    }
}

/**
 * Clone the level.
 * @param  level The level to clone.
 * @return       The new level.
 */
static Level *clone (Level *level)
{
    Level *newlevel; /* the level clone */
    int c; /* general counter */

    /* reserve memory for the new level */
    newlevel = new_Level ();

    /* copy the cell information and contents */
    for (c = 0; c < 192; ++c) {
	newlevel->cells[c] = level->cells[c];
	newlevel->items[c] = level->items[c]
	    ? level->items[c]->clone (level->items[c])
	    : NULL;
	newlevel->robots[c] = level->robots[c]
	    ? level->robots[c]->clone (level->robots[c])
	    : NULL;
    }

    /* copy the level-wide data */
    newlevel->readers = level->readers;
    newlevel->cards = level->cards;
    newlevel->spawners = level->spawners;
    newlevel->robotcount = level->robotcount;
    newlevel->turns = level->turns;

    /* return the new level */
    return newlevel;
}

/**
 * Clear the level.
 * @param  level The level to clear.
 */
static void clear (Level *level)
{
    int c; /* general counter */
    Robot *robot; /* pointer to robot object */
    Item *item; /* pointer to item object */

    /* clear out cell data */
    for (c = 0; c < 192; ++c) {
	level->cells[c] = NULL;
	if ((item = level->items[c])) {
	    item->destroy (item);
	    level->items[c] = NULL;
	}
	if ((robot = level->robots[c])) {
	    robot->destroy (robot);
	    level->robots[c] = NULL;
	}
    }

    /* clear out level wide data */
    level->readers = 0;
    level->cards = 0;
    level->spawners = 0;
    level->robotcount = 0;
    level->turns = 0;
}

/**
 * Write the level to an already open file.
 * @param  level  The level to write.
 * @param  output The output file handle.
 * @return        1 if successful, 0 on failure.
 */
static int write (Level *level, FILE *output)
{
    int c, /* general counter */
	zero = 0, /* a zero value we can pass a pointer to */
	r = 1; /* return code */
    Robot *robot; /* pointer to a robot */
    Item *item; /* pointer to an item */

    /* write the cell types */
    for (c = 0; c < 192; ++c)
	if (! level->cells[c])
	    r = r && writeint (&zero, output);
	else
	    r = r && writeint (&level->cells[c]->type, output);

    /* write the items */
    for (c = 0; c < 192; ++c)
	if (! (item = level->items[c]))
	    r = r && writeint (&zero, output);
	else
	    r = r && writeint (&level->items[c]->type, output);

    /* write the robots */
    for (c = 0; c < 192; ++c)
	if (! (robot = level->robots[c]))
	    r = r && writeint (&zero, output);
	else {
	    r = r && writeint (&level->robots[c]->type, output);
	    r = r && robot->write (robot, output);
	}

    /* write the number of turns taken */
    r = r && writeint (&level->turns, output);

    /* return success */
    return r;
}

/**
 * Read the level from an already open file.
 * @param  level The level to read.
 * @param  input The input file handle.
 * @return       1 if successful, 0 on failure.
 */
static int read (Level *level, FILE *input)
{
    int c, /* general counter */
	type, /* type ID read from the file */
	r = 1; /* return code */

    /* initialise cache attributes */
    level->readers = 0;
    level->cards = 0;
    level->spawners = 0;
    level->robotcount = 0;

    /* read the cell types */
    for (c = 0; c < 192; ++c)
	if ((r = r && readint (&type, input)) && type) {
	    level->cells[c] = get_Cell (type);
	    if (type == CELL_READER)
		++level->readers;
	}

    /* read the items */
    for (c = 0; c < 192; ++c)
	if ((r = r && readint (&type, input)) && type) {
	    if (! (level->items[c] = new_Item (type)))
		fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
	    if (type == ITEM_CARD)
		++level->cards;
	    else if (type == ITEM_SPAWNER)
		++level->spawners;
	}

    /* read the robots */
    for (c = 0; c < 192; ++c)
	if ((r = r && readint (&type, input)) && type) {
	    if (! (level->robots[c] = new_Robot (type)))
		fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
	    level->robots[c]->read (level->robots[c], input);
	    if (type != ROBOT_GUARD)
		++level->robotcount;
	}

    /* read the number of turns taken */
    readint (&level->turns, input);

    /* return success */
    return r;
}

/*----------------------------------------------------------------------
 * Top Level Function Definitions.
 */

/**
 * Level contstructor function.
 * @return The new level.
 */
Level *new_Level (void)
{
    Level *level; /* level to return */
    int c; /* general counter */

    /* reserve memory for the level */
    if (! (level = malloc (sizeof (Level))))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* initialise attriutes */
    for (c = 0; c < 192; ++c) {
	level->cells[c] = NULL;
	level->items[c] = NULL;
	level->robots[c] = NULL;
    }
    level->readers = 0;
    level->cards = 0;
    level->spawners = 0;
    level->robotcount = 0;
    level->turns = 0;

    /* initialise the methods */
    level->destroy = destroy;
    level->clone = clone;
    level->clear = clear;
    level->write = write;
    level->read = read;

    /* return the new level */
    return level;
}

