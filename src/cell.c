/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker 2022.
 * Map cell module.
 */

/*----------------------------------------------------------------------
 * Required Headers.
 */

/* ANSI C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* project-specific headers */
#include "cell.h"
#include "robot.h"
#include "level.h"
#include "fatal.h"


/*----------------------------------------------------------------------
 * Data Definitions.
 */

/** @var cells All the cell types. */
static Cell *cells[12];

/** @var cellnames A list of cell names */
static char *cellnames[12] = {
    "Floor",
    "Wall",
    "Conveyor Belt", /* north */
    "Conveyor Belt", /* east */
    "Conveyor Belt", /* south */
    "Conveyor Belt", /* west */
    "Turntable", /* clockwise */
    "Turntable", /* anti-clockwise */
    "Teleporter",
    "Forcefield",
    "Generator",
    "Card Reader"
};

/** @var celldata A table of cell attributes. */
static int celldata[12][3] = {
    {1, 1, 0}, /* can move or put on floor */
    {0, 0, 1}, /* can shoot wall */
    {1, 1, 0}, /* can move or put on north conveyor */
    {1, 1, 0}, /* can move or put on east conveyor */
    {1, 1, 0}, /* can move or put on south conveyor */
    {1, 1, 0}, /* can move or put on west conveyor */
    {1, 1, 0}, /* can move or put on clockwise turntable */
    {1, 1, 0}, /* can move or put on anti-clockwise turntable */
    {1, 1, 0}, /* can move or put on teleporter */
    {1, 1, 1}, /* can move on, put on or shoot forcefield */
    {0, 0, 1}, /* can shoot forcefield generator */
    {0, 1, 1} /* can put on card reader or shoot it */
};

/** @var xoffset The x offset for each facing. */
static int xoffset[] = {
    0, /* north */
    +1, /* east */
    0, /* south */
    -1 /* west */
};

/** @var xoffset The x offset for each facing. */
static int yoffset[] = {
    -1, /* north */
    0, /* east */
    +1, /* south */
    0 /* west */
};

/*----------------------------------------------------------------------
 * Level 2 Function Definitions.
 */

/**
 * Return the nearest teleport to an X/Y coordinate.
 * @param  level The level to scan.
 * @param  x     The origin x coordinate.
 * @param  y     The origin y coordinate.
 * @return       The destination square.
 */
static int nearestteleport (Level *level, int x, int y)
{
    int c, /* cell counter */
	dest, /* destination cell */
	nearest = 0, /* distance to the nearest teleport */
	dist; /* distance to the scanned cell */

    /* dest defaults to current square */
    dest = x + 16 * y;

    /* scan through the map for other teleports */
    for (c = 0; c < 192; ++c) {

	/* skip non-teleports and the start cell */
	if (c % 16 == x && c / 16 == y)
	    continue;
	if (level->cells[c]->type != CELL_TELEPORTER)
	    continue;

	/* find distance between this teleport and us */
	dist = abs (c % 16 - x);
	if (abs (c / 16 - y) > dist)
	    dist = abs (c / 16 - y);

	/* set destination if it is closest found */
	if (dist < nearest || nearest == 0) {
	    dest = c;
	    nearest = dist;
	}
    }

    /* return the destination */
    return dest;
}

/*----------------------------------------------------------------------
 * Level 1 Function Definitions.
 */

/**
 * Move a robot in a given direction.
 * @param  level The level map.
 * @param  x     The x coordinate of the cell.
 * @param  y     The y coordinate of the cell.
 * @param  h     Horizontal movement.
 * @param  v     Vertical movement.
 * @return       1 if something happened, 0 if not.
 */
static int moverobot (Level *level, int x, int y, int h, int v)
{
    int origin, /* origin square */
	dest; /* destination square */

    /* initialise convenience variables */
    origin = x + 16 * y;
    dest = (x + h) + 16 * (y + v);

    /* validate move */
    if (! level->robots[origin])
	return 0; /* the robot is no longer here */
    if (level->robots[origin]->status == ROBOT_CONVEYED)
	return 0; /* a robot can be moved only once per turn */
    if (x + h < 0 || x + h > 15 || y + v < 0 || y + v > 11)
	return 0; /* cannot move off the map */
    if (level->items[dest])
	return 0; /* cannot move on to items */
    if (level->robots[dest])
	return 0; /* cannot move on to other robots */
    if (! level->cells[dest]->allowmove)
	return 0; /* cannot move on to some cells */

    /* move the robot */
    level->robots[origin]->x += h;
    level->robots[origin]->y += v;
    level->robots[origin]->status = ROBOT_CONVEYED;
    level->robots[dest] = level->robots[origin];
    level->robots[origin] = NULL;

    /* move any item it is carrying */
    level->items[dest] = level->items[origin];
    level->items[origin] = NULL;

    /* return 1 to say the robot has moved */
    return 1;
}

/**
 * Move an item in the given direction.
 * @param  x     The x coordinate of the cell.
 * @param  y     The y coordinate of the cell.
 * @param  h     Horizontal movement.
 * @param  v     Vertical movement.
 * @return       1 if something happened, 0 if not.
 */
static int moveitem (Level *level, int x, int y, int h, int v)
{
    int origin, /* origin square */
	dest; /* destination square */

    /* initialise convenience variables */
    origin = x + 16 * y;
    dest = (x + h) + 16 * (y + v);

    /* validate move */
    if (level->robots[origin])
	return 0; /* the item is being carried by a robot */
    if (! level->items[origin])
	return 0; /* the item is no longer here */
    if (level->items[origin]->status == ITEM_CONVEYED)
	return 0; /* the item has already been pushed this turn */
    if (x + h < 0 || x + h > 15 || y + v < 0 || y + v > 11)
	return 0; /* cannot move off the map */
    if (level->items[dest])
	return 0; /* cannot move onto other items */
    if (level->robots[dest])
	return 0; /* cannot move on to robots */
    if (! level->cells[dest]->allowput)
	return 0; /* cannot move on to some cells */

    /* move the item */
    level->items[dest] = level->items[origin];
    level->items[dest]->status = ITEM_CONVEYED;
    level->items[origin] = NULL;

    /* return 1 to say the robot has moved */
    return 1;
}


/*----------------------------------------------------------------------
 * Public Method Function Definitions.
 */

/**
 * A simple stub function for cells that have no effect on robots.
 * @param  level The level map.
 * @param  x     The x coordinate of the cell.
 * @param  y     The y coordinate of the cell.
 * @return       1 if something happened, 0 if not.
 */
static int noeffect (Level *level, int x, int y)
{
    return 0;
}

/**
 * The effect when a robot moves or its on to a north conveyor.
 * @param  level The level map.
 * @param  x     The x coordinate of the cell.
 * @param  y     The y coordinate of the cell.
 * @return       1 if something happened, 0 if not.
 */
static int moverobotnorth (Level *level, int x, int y)
{
    return moverobot (level, x, y, 0, -1);
}

/**
 * The effect when an item is dropped or pushed onto a north conveyor.
 * @param  level The level map.
 * @param  x     The x coordinate of the cell.
 * @param  y     The y coordinate of the cell.
 * @return       1 if something happened, 0 if not.
 */
static int moveitemnorth (Level *level, int x, int y)
{
    return moveitem (level, x, y, 0, -1);
}

/**
 * The effect when a robot moves onto a east conveyor.
 * @param  level The level map.
 * @param  x     The x coordinate of the cell.
 * @param  y     The y coordinate of the cell.
 * @return       1 if something happened, 0 if not.
 */
static int moveroboteast (Level *level, int x, int y)
{
    return moverobot (level, x, y, 1, 0);
}

/**
 * The effect when an item is dropped or pushed onto a east conveyor.
 * @param  level The level map.
 * @param  x     The x coordinate of the cell.
 * @param  y     The y coordinate of the cell.
 * @return       1 if something happened, 0 if not.
 */
static int moveitemeast (Level *level, int x, int y)
{
    return moveitem (level, x, y, 1, 0);
}

/**
 * The effect when a robot moves onto a south conveyor.
 * @param  level The level map.
 * @param  x     The x coordinate of the cell.
 * @param  y     The y coordinate of the cell.
 * @return       1 if something happened, 0 if not.
 */
static int moverobotsouth (Level *level, int x, int y)
{
    return moverobot (level, x, y, 0, 1);
}

/**
 * The effect when an item is dropped or pushed onto a south conveyor.
 * @param  level The level map.
 * @param  x     The x coordinate of the cell.
 * @param  y     The y coordinate of the cell.
 * @return       1 if something happened, 0 if not.
 */
static int moveitemsouth (Level *level, int x, int y)
{
    return moveitem (level, x, y, 0, 1);
}

/**
 * The effect when a robot moves onto a west conveyor.
 * @param  level The level map.
 * @param  x     The x coordinate of the cell.
 * @param  y     The y coordinate of the cell.
 * @return       1 if something happened, 0 if not.
 */
static int moverobotwest (Level *level, int x, int y)
{
    return moverobot (level, x, y, -1, 0);
}

/**
 * The effect when an item is dropped or pushed onto a west conveyor.
 * @param  level The level map.
 * @param  x     The x coordinate of the cell.
 * @param  y     The y coordinate of the cell.
 * @return       1 if something happened, 0 if not.
 */
static int moveitemwest (Level *level, int x, int y)
{
    return moveitem (level, x, y, -1, 0);
}

/**
 * The effect when a robot moves onto a clockwise turntable.
 * @param  level The level map.
 * @param  x     The x coordinate of the cell.
 * @param  y     The y coordinate of the cell.
 * @return       1 if something happened, 0 if not.
 */
static int turnrobotclockwise (Level *level, int x, int y)
{
    Robot *robot; /* pointer to robot */
    robot = level->robots[x + 16 * y];
    if (! robot)
	return 0; /* there is no robot */
    robot->facing = (robot->facing + 1) & 3;
    return 1;
}

/**
 * The effect when a robot moves onto a clockwise turntable.
 * @param  level The level map.
 * @param  x     The x coordinate of the cell.
 * @param  y     The y coordinate of the cell.
 * @return       1 if something happened, 0 if not.
 */
static int turnrobotanticlockwise (Level *level, int x, int y)
{
    Robot *robot; /* pointer to robot */
    robot = level->robots[x + 16 * y];
    if (! robot)
	return 0; /* there is no robot */
    robot->facing = (robot->facing - 1) & 3;
    return 1;
}

/**
 * The effect when a robot moves onto a teleporter.
 * @param  level The level map.
 * @param  x     The x coordinate of the cell.
 * @param  y     The y coordinate of the cell.
 * @return       1 if something happened, 0 if not.
 */
static int teleportrobot (Level *level, int x, int y)
{
    int dest; /* destination teleport */

    /* reject immediately if robot has teleported already */
    if (! level->robots[x + 16 * y])
	return 0; /* there is no robot here */
    if (level->robots[x + 16 * y]->status == ROBOT_TELEPORTED)
	return 0; /* this robot has just teleported */

    /* find the nearest teleport */
    dest = nearestteleport (level, x, y);

    /* check destination */
    if (dest == x + 16 * y)
	return 0; /* we didn't find another teleport */
    if (level->items[dest])
	return 0; /* there's an item in the way */
    if (level->robots[dest])
	return 0; /* there's a robot in the way */

    /* teleport robot and item */
    level->robots[dest] = level->robots[x + 16 * y];
    level->robots[dest]->x = dest % 16;
    level->robots[dest]->y = dest / 16;
    level->robots[dest]->status = ROBOT_TELEPORTED;
    level->robots[x + 16 * y] = NULL;
    level->items[dest] = level->items[x + 16 * y];
    level->items[x + 16 * y] = NULL;
    return 1;
}

/**
 * The effect when an item is dropped or pushed onto a teleporter.
 * @param  level The level map.
 * @param  x     The x coordinate of the cell.
 * @param  y     The y coordinate of the cell.
 * @return       1 if something happened, 0 if not.
 */
static int teleportitem (Level *level, int x, int y)
{
    int dest; /* destination teleport */

    /* reject immediately under various conditions */
    if (level->robots[x + 16 * y])
	return 0; /* the item is being carried */
    if (level->items[x + 16 * y]->status == ITEM_TELEPORTED)
	return 0; /* the item has been teleported already */

    /* find the nearest teleport */
    dest = nearestteleport (level, x, y);

    /* check destination */
    if (dest == x + 16 * y)
	return 0; /* we didn't find another teleport */
    if (level->items[dest])
	return 0; /* there's an item in the way */
    if (level->robots[dest])
	return 0; /* there's a robot in the way */

    /* teleport item */
    level->items[dest] = level->items[x + 16 * y];
    level->items[dest]->status = ITEM_TELEPORTED;
    level->items[x + 16 * y] = NULL;
    return 1;
}

/**
 * The effect when a robot moves onto a forcefield.
 * @param  level The level map.
 * @param  x     The x coordinate of the cell.
 * @param  y     The y coordinate of the cell.
 * @return       1 if something happened, 0 if not.
 */
static int destroyrobot (Level *level, int x, int y)
{
    /* reject immediately if no robot is here */
    if (! level->robots[x + 16 * y])
	return 0;

    /* destroy robot and item */
    level->robots[x + 16 * y]->status = ROBOT_DESTROYED;
    if (level->items[x + 16 * y])
	level->items[x + 16 * y]->status = ITEM_DESTROYED;
    return 1;
}

/**
 * The effect when an item is dropped or pushed onto a forcefield.
 * @param  level The level map.
 * @param  x     The x coordinate of the cell.
 * @param  y     The y coordinate of the cell.
 * @return       1 if something happened, 0 if not.
 */
static int destroyitem (Level *level, int x, int y)
{
    if (! level->items[x + 16 * y])
	return 0; /* no item is here */
    level->items[x + 16 * y]->status = ITEM_DESTROYED;
    return 1;
}

/**
 * The effect when a forcefield generator is shot.
 * @param  level The level map.
 * @param  x     The x coordinate of the cell.
 * @param  y     The y coordinate of the cell.
 * @return       1 if something happened, 0 if not.
 */
static int deactivateforcefield (Level *level, int x, int y)
{
    int f, /* facing count */
	xf, /* x facing offset */
	yf, /* y facing offset */
	xc, /* x check position */
	yc, /* y check position */
	changes = 0; /* 1 if there were changes to the level */

    /* check each of the four directions for forcefields */
    for (f = 0; f < 4; ++f) {

	/* work out facing offset and initial check position */
	xf = xoffset[f];
	yf = yoffset[f];
	xc = x + xf;
	yc = y + yf;

	/* destroy every adjacent forcefield in that direction */
	while (xc > 0 && xc <= 15 && yc >= 0 && yc <= 11 &&
	       level->cells[xc + 16 * yc]->type == CELL_FORCEFIELD) {
	    level->cells[xc + 16 * yc] = get_Cell (CELL_FLOOR);
	    xc += xf;
	    yc += yf;
	    changes = 1;
	}
    }

    /* return change state */
    return changes;
}

/**
 * The effect when an item is put on a card reader.
 * @param  level The level map.
 * @param  x     The x coordinate of the cell.
 * @param  y     The y coordinate of the cell.
 * @return       1 if something happened, 0 if not.
 */
static int checkinsertcard (Level *level, int x, int y)
{
    return 0;
}

/*----------------------------------------------------------------------
 * Top Level Function Definitions.
 */

/**
 * Cell contstructor function.
 * @param  type The cell type 1..12.
 * @return      The new cell.
 */
Cell *get_Cell (int type)
{
    /* have we already instantiated this cell */
    if (cells[type - 1])
	return cells[type - 1];

    /* otherwise instantiate the cell */
    if (! (cells[type - 1] = malloc (sizeof (Cell))))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* initialise the attributes */
    cells[type - 1]->type = type;
    cells[type - 1]->name = cellnames[type - 1];
    cells[type - 1]->allowmove = celldata[type - 1][0];
    cells[type - 1]->allowput = celldata[type - 1][1];
    cells[type - 1]->allowshoot = celldata[type - 1][2];

    /* initialise the effects to default of nothing */
    cells[type - 1]->onrobot = noeffect;
    cells[type - 1]->onitem = noeffect;
    cells[type - 1]->onshoot = noeffect;

    /* set the effects for each cell type */
    switch (type) {
    case CELL_CONVEYOR_NORTH:
	cells[type - 1]->onrobot = moverobotnorth;
	cells[type - 1]->onitem = moveitemnorth;
	break;
    case CELL_CONVEYOR_EAST:
	cells[type - 1]->onrobot = moveroboteast;
	cells[type - 1]->onitem = moveitemeast;
	break;
    case CELL_CONVEYOR_SOUTH:
	cells[type - 1]->onrobot = moverobotsouth;
	cells[type - 1]->onitem = moveitemsouth;
	break;
    case CELL_CONVEYOR_WEST:
	cells[type - 1]->onrobot = moverobotwest;
	cells[type - 1]->onitem = moveitemwest;
	break;
    case CELL_TURNTABLE_CLOCKWISE:
	cells[type - 1]->onrobot = turnrobotclockwise;
	break;
    case CELL_TURNTABLE_ANTICLOCKWISE:
	cells[type - 1]->onrobot = turnrobotanticlockwise;
	break;
    case CELL_TELEPORTER:
	cells[type - 1]->onrobot = teleportrobot;
	cells[type - 1]->onitem = teleportitem;
	break;
    case CELL_FORCEFIELD:
	cells[type - 1]->onrobot = destroyrobot;
	cells[type - 1]->onitem = destroyitem;
	break;
    case CELL_GENERATOR:
	cells[type - 1]->onshoot = deactivateforcefield;
	break;
    case CELL_READER:
	cells[type - 1]->onitem = checkinsertcard;
	break;
    }

    /* return the cell */
    return cells[type - 1];
}

/**
 * Destroy all cells.
 */
void destroy_Cells (void)
{
    int c; /* general counter */
    for (c = 0; c < 12; ++c)
	if (cells[c])
	    free (cells[c]);
}
