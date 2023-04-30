/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker 2022.
 * Robot module.
 */

/*----------------------------------------------------------------------
 * Headers.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* project-specific headers */
#include "robot.h"
#include "fatal.h"
#include "utils.h"


/*----------------------------------------------------------------------
 * Level 1 Function Definitions.
 */

/**
 * Initialise a robot's attributes.
 * @param robot The robot to initialise.
 */
static void initialiseattributes (Robot *robot)
{
    int c; /* general loop counter */
    *robot->name = '\0';
    robot->type = 0;
    robot->ramsize = 0;
    robot->haswalker = 0;
    robot->hasspring = 0;
    robot->hasphaser = 0;
    robot->hasinventory = 0;
    robot->rom = 0;
    for (c = 0; c < 8; ++c)
	robot->ram[c] = 0;
    robot->x = 0xff;
    robot->y = 0xff;
    robot->facing = 0;
    robot->status = ROBOT_INERT;
}

/*----------------------------------------------------------------------
 * Public Methods.
 */

/**
 * Destroy the robot when no longer needed.
 * @param robot The robot to destroy.
 */
static void destroy (Robot *robot)
{
    if (robot)
	free (robot);
}

/**
 * Clone the robot.
 * @param  robot The robot to clone.
 * @return       The new robot.
 */
static Robot *clone (Robot *robot)
{
    Robot *newrobot; /* the new robot */
    int c; /* generic counter */

    /* reserve memory for the new robot */
    if (! (newrobot = new_Robot (robot->type)))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* copy the attributes */
    strcpy (newrobot->name, robot->name);
    newrobot->ramsize = robot->ramsize;
    newrobot->haswalker = robot->haswalker;
    newrobot->hasspring = robot->hasspring;
    newrobot->hasphaser = robot->hasphaser;
    newrobot->hasinventory = robot->hasinventory;
    newrobot->rom = robot->rom;
    for (c = 0; c < 8; ++c)
	newrobot->ram[c] = robot->ram[c];
    newrobot->x = robot->x;
    newrobot->y = robot->y;
    newrobot->facing = robot->facing;
    newrobot->status = robot->status;

    /* return the new robot */
    return newrobot;
}

/**
 * Clear the robot data.
 * @param robot The robot object to clear.
 */
static void clear (Robot *robot)
{
    initialiseattributes (robot);
}

/**
 * Write the robot to an already open file.
 * @param robot The robot to write.
 * @param output The output file handle.
 * @return 1 if successful, 0 on failure.
 */
static int write (Robot *robot, FILE *output)
{
    int r, /* return code */
	c; /* generic counter */

    /* write the name */
    r = writestring (robot->name, output);

    /* write the static attributes */
    r = r && writeint (&robot->type, output)
	&& writeint (&robot->ramsize, output)
	&& writeint (&robot->haswalker, output)
	&& writeint (&robot->hasspring, output)
	&& writeint (&robot->hasphaser, output)
	&& writeint (&robot->hasinventory, output)
	&& writeint (&robot->rom, output);

    /* write the changing attributes */
    for (c = 0; c < robot->ramsize; ++c)
	r = r && writeint (&robot->ram[c], output);
    r = r && writeint (&robot->x, output)
	&& writeint (&robot->y, output)
	&& writeint (&robot->facing, output);

    /* return success code */
    return r;
}

/**
 * Read the robot from an already open file.
 * @param robot The robot to read.
 * @param input The input file handle.
 * @return 1 if successful, 0 on failure.
 */
static int read (Robot *robot, FILE *input)
{
    int r, /* return code */
	c; /* generic counter */

    /* read the name */
    r = readstring (robot->name, input);

    /* read the static attributes */
    r = r && readint (&robot->type, input)
	&& readint (&robot->ramsize, input)
	&& readint (&robot->haswalker, input)
	&& readint (&robot->hasspring, input)
	&& readint (&robot->hasphaser, input)
	&& readint (&robot->hasinventory, input)
	&& readint (&robot->rom, input);

    /* read the changing attributes */
    for (c = 0; c < 8; ++c)
	if (c < robot->ramsize)
	    r = r && readint (&robot->ram[c], input);
	else
	    robot->ram[c] = 0;
    r = r && readint (&robot->x, input)
	&& readint (&robot->y, input)
	&& readint (&robot->facing, input);

    /* return success code */
    return r;
}

/**
 * Perform an action.
 * @param robot  The robot to act.
 * @param level  The level on which the action is performed.
 * @param action The action to perform.
 */
static void act (Robot *robot, Level *level, Action *action)
{
}

/*----------------------------------------------------------------------
 * Top-level Function Declarations.
 */

/**
 * Robot contstructor function.
 * @param  type The robot type 0 (for guard) or 1..6.
 * @return The new robot.
 */
Robot *new_Robot (int type)
{
    Robot *robot; /* new robot */

    /* attempt to reserve memory */
    if (! (robot = malloc (sizeof (Robot))))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* initialise attributes */
    initialiseattributes (robot);
    robot->type = type;

    /* initialise methods */
    robot->destroy = destroy;
    robot->clone = clone;
    robot->clear = clear;
    robot->write = write;
    robot->read = read;
    robot->act = act;

    /* return the new robot */
    return robot;
}


