/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Action Header.
 */

/*----------------------------------------------------------------------
 * Required Headers.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* project specific headers */
#include "action.h"
#include "robot.h"
#include "level.h"
#include "cell.h"
#include "item.h"
#include "fatal.h"


/*----------------------------------------------------------------------
 * Data Definitions.
 */

/** @var actions The list of actions. */
static Action *actions[12];

/** @var names The names of the actions */
static char *names[] = {
    "Step Forward",
    "Step Backward",
    "Step Left",
    "Step Right",
    "Sprint",
    "Leap",
    "Turn Left",
    "Turn Right",
    "Turn About",
    "Take",
    "Drop",
    "Shoot"
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
 * Level 1 Function Definitions.
 */

/**
 * Check for robot collisions.
 * Destroy a player robot if it collides with a guard robot.
 * @param  level  The level to check.
 * @param  
 */
static int checkrobotcollision (Robot *origin, Robot *dest)
{
    /* if no robot, no collision */
    if (! origin || ! dest)
	return 0;

    /* if player robot shunts guard, player dies */
    if (origin->type != ROBOT_GUARD && dest->type == ROBOT_GUARD) {
	origin->status = ROBOT_DESTROYED;
	return 1;
    }

    /* if guard robot shunts player, player dies */
    if (origin->type == ROBOT_GUARD && dest->type != ROBOT_GUARD) {
	dest->status = ROBOT_DESTROYED;
	return 1;
    }

    /* player-on-player or guard-on-guard collision causes no damage */
    return 1;
}

/*----------------------------------------------------------------------
 * Public Method Function Definitions.
 */

/**
 * Execute a Step Forward action.
 * @param  action The action to execute.
 * @param  robot  The robot that is executing the action.
 * @param  level  The level on which the action takes place.
 * @return        1 if the action was successful.
 */
static int stepforward (Action *action, Robot *robot, Level *level)
{
    int xo, /* x coordinate of origin */
	yo, /* y coordinate of origin */
	xd, /* x coordinate of destination */
	yd, /* y coordinate of destination */
	xp, /* x coordinate of pushed item */
	yp, /* y coordinate of pushed item */
	pushing = 0; /* 1 if pushing an item */

    /* work out coordinates */
    xo = robot->x;
    yo = robot->y;
    xd = xo + xoffset[robot->facing];
    yd = yo + yoffset[robot->facing];
    xp = xd + xoffset[robot->facing];
    yp = yd + yoffset[robot->facing];

    /* reject invalid moves */
    if (xd < 0 || xd > 15 || yd < 0 || yd > 11)
	return 0; /* cannot move off the map */
    if (! level->cells[xd + 16 * yd]->allowmove)
	return 0; /* cannot move on to certain blocks */
    if (checkrobotcollision (robot, level->robots[xd + 16 * yd]))
	return 0;

    /* are we pushing an item? */
    if (level->items[xd + 16 * yd]) {
	pushing = 1;
	if (level->items[xp + 16 * yp])
	    return 0; /* cannot push two items in a row */
	if (xp < 0 || xp > 15 || yp < 0 || yp > 11)
	    return 0; /* cannot move off the map */
	if (! level->cells[xp + 16 * yp]->allowput)
	    return 0; /* cannot push onto certain blocks */
	if (level->robots[xp + 16 * yp])
	    return 0; /* cannot move on to robots */
    }

    /* move robot in specified direction */
    if (pushing)
	level->items[xp + 16 * yp] = level->items[xd + 16 * yd];
    level->robots[xd + 16 * yd] = robot;
    level->robots[xo + 16 * yo] = NULL;
    level->items[xd + 16 * yd] = level->items[xo + 16 * yo];
    level->items[xo + 16 * yo] = NULL;
    robot->x = xd;
    robot->y = yd;

    /* return success */
    return 1;
}

/**
 * Execute a Step Backward action.
 * @param  action The action to execute.
 * @param  robot  The robot that is executing the action.
 * @param  level  The level on which the action takes place.
 * @return        1 if the action was successful.
 */
static int stepbackward (Action *action, Robot *robot, Level *level)
{
    int xo, /* x coordinate of origin */
	yo, /* y coordinate of origin */
	xd, /* x coordinate of destination */
	yd; /* y coordinate of destination */

    /* work out coordinates */
    xo = robot->x;
    yo = robot->y;
    xd = xo + xoffset[robot->facing ^ 2];
    yd = yo + yoffset[robot->facing ^ 2];

    /* reject invalid moves */
    if (xd < 0 || xd > 15 || yd < 0 || yd > 11)
	return 0; /* cannot move off the map */
    if (! level->cells[xd + 16 * yd]->allowmove)
	return 0; /* cannot move on to certain blocks */
    if (level->items[xd + 16 * yd])
	return 0; /* cannot push items backwards */
    if (checkrobotcollision (robot, level->robots[xd + 16 * yd]))
	return 0;
    if (level->items[xd + 16 * yd])
	return 0; /* cannot push items backwards */

    /* move robot in specified direction */
    level->robots[xd + 16 * yd] = robot;
    level->robots[xo + 16 * yo] = NULL;
    level->items[xd + 16 * yd] = level->items[xo + 16 * yo];
    level->items[xo + 16 * yo] = NULL;
    robot->x = xd;
    robot->y = yd;

    /* return success */
    return 1;
}

/**
 * Execute a Step Left action.
 * @param  action The action to execute.
 * @param  robot  The robot that is executing the action.
 * @param  level  The level on which the action takes place.
 * @return        1 if the action was successful.
 */
static int stepleft (Action *action, Robot *robot, Level *level)
{
    int xo, /* x coordinate of origin */
	yo, /* y coordinate of origin */
	xd, /* x coordinate of destination */
	yd; /* y coordinate of destination */

    /* work out coordinates */
    xo = robot->x;
    yo = robot->y;
    xd = xo + xoffset[(robot->facing - 1) & 3];
    yd = yo + yoffset[(robot->facing - 1) & 3];

    /* reject invalid moves */
    if (! robot->haswalker &&
	(! level->items[xo + 16 * yo] ||
	 level->items[xo + 16 * yo]->type != ITEM_WALKER))
	return 0; /* need a walker for lateral movement */
    if (xd < 0 || xd > 15 || yd < 0 || yd > 11)
	return 0; /* cannot move off the map */
    if (! level->cells[xd + 16 * yd]->allowmove)
	return 0; /* cannot move on to certain blocks */
    if (checkrobotcollision (robot, level->robots[xd + 16 * yd]))
	return 0;
    if (level->items[xd + 16 * yd])
	return 0; /* cannot push items aside */

    /* move robot in specified direction */
    level->robots[xd + 16 * yd] = robot;
    level->robots[xo + 16 * yo] = NULL;
    level->items[xd + 16 * yd] = level->items[xo + 16 * yo];
    level->items[xo + 16 * yo] = NULL;
    robot->x = xd;
    robot->y = yd;

    /* return success */
    return 1;
}

/**
 * Execute a Step Right action.
 * @param  action The action to execute.
 * @param  robot  The robot that is executing the action.
 * @param  level  The level on which the action takes place.
 * @return        1 if the action was successful.
 */
static int stepright (Action *action, Robot *robot, Level *level)
{
    int xo, /* x coordinate of origin */
	yo, /* y coordinate of origin */
	xd, /* x coordinate of destination */
	yd; /* y coordinate of destination */

    /* work out coordinates */
    xo = robot->x;
    yo = robot->y;
    xd = xo + xoffset[(robot->facing + 1) & 3];
    yd = yo + yoffset[(robot->facing + 1) & 3];

    /* reject invalid moves */
    if (! robot->haswalker &&
	(! level->items[xo + 16 * yo] ||
	 level->items[xo + 16 * yo]->type != ITEM_WALKER))
	return 0; /* need a walker for lateral movement */
    if (xd < 0 || xd > 15 || yd < 0 || yd > 11)
	return 0; /* cannot move off the map */
    if (! level->cells[xd + 16 * yd]->allowmove)
	return 0; /* cannot move on to certain blocks */
    if (checkrobotcollision (robot, level->robots[xd + 16 * yd]))
	return 0;
    if (level->items[xd + 16 * yd])
	return 0; /* cannot push items aside */

    /* move robot in specified direction */
    level->robots[xd + 16 * yd] = robot;
    level->robots[xo + 16 * yo] = NULL;
    level->items[xd + 16 * yd] = level->items[xo + 16 * yo];
    level->items[xo + 16 * yo] = NULL;
    robot->x = xd;
    robot->y = yd;

    /* return success */
    return 1;
}

/**
 * Execute a Sprint action. This is exactly the same as a Step Forward
 * action, except that it is called twice from the action module.
 * @param  action The action to execute.
 * @param  robot  The robot that is executing the action.
 * @param  level  The level on which the action takes place.
 * @return        1 if the action was successful.
 */
static int sprint (Action *action, Robot *robot, Level *level)
{
    /* sprint is unavailable if robot is using a walker */
    if (robot->haswalker ||
	(level->items[robot->x + 16 * robot->y] &&
	 level->items[robot->x + 16 * robot->y]->type == ITEM_WALKER))
	return 0;
    return stepforward (action, robot, level);
}

/**
 * Execute a Leap action.
 * @param  action The action to execute.
 * @param  robot  The robot that is executing the action.
 * @param  level  The level on which the action takes place.
 * @return        1 if the action was successful.
 */
static int leap (Action *action, Robot *robot, Level *level)
{
    int xo, /* x coordinate of origin */
	yo, /* y coordinate of origin */
	xd, /* x coordinate of destination */
	yd, /* y coordinate of destination */
	xl, /* x coordinate of leapt cell */
	yl; /* y coordinate of leapt cell */

    /* work out coordinates */
    xo = robot->x;
    yo = robot->y;
    xl = xo + xoffset[robot->facing];
    yl = yo + yoffset[robot->facing];
    xd = xl + xoffset[robot->facing];
    yd = yl + yoffset[robot->facing];

    /* reject invalid moves */
    if (! robot->hasspring &&
	(! level->items[xo + 16 * yo] ||
	 level->items[xo + 16 * yo]->type != ITEM_SPRING))
	return 0; /* need a spring to leap */
    if (xd < 0 || xd > 15 || yd < 0 || yd > 11)
	return 0; /* cannot move off the map */
    if (! level->cells[xl + 16 * yl]->allowmove)
	return 0; /* cannot leap over to certain blocks */
    if (! level->cells[xd + 16 * yd]->allowmove)
	return 0; /* cannot move on to certain blocks */
    if (level->robots[xd + 16 * yd])
	return 0; /* cannot move on to robots */
    if (level->items[xd + 16 * yd])
	return 0; /* cannot leap on to items */

    /* move robot in specified direction */
    level->robots[xd + 16 * yd] = robot;
    level->robots[xo + 16 * yo] = NULL;
    level->items[xd + 16 * yd] = level->items[xo + 16 * yo];
    level->items[xo + 16 * yo] = NULL;
    robot->x = xd;
    robot->y = yd;

    /* return success */
    return 1;
}

/**
 * Execute a Turn Left action.
 * @param  action The action to execute.
 * @param  robot  The robot that is executing the action.
 * @param  level  The level on which the action takes place.
 * @return        1 if the action was successful.
 */
static int turnleft (Action *action, Robot *robot, Level *level)
{
    robot->facing = (robot->facing - 1) & 3;
    return 1;
}

/**
 * Execute a Turn Right action.
 * @param  action The action to execute.
 * @param  robot  The robot that is executing the action.
 * @param  level  The level on which the action takes place.
 * @return        1 if the action was successful.
 */
static int turnright (Action *action, Robot *robot, Level *level)
{
    robot->facing = (robot->facing + 1) & 3;
    return 1;
}

/**
 * Execute a Turn About action.
 * @param  action The action to execute.
 * @param  robot  The robot that is executing the action.
 * @param  level  The level on which the action takes place.
 * @return        1 if the action was successful.
 */
static int turnabout (Action *action, Robot *robot, Level *level)
{
    robot->facing ^= 2;
    return 1;
}

/**
 * Execute a Take Item action.
 * @param  action The action to execute.
 * @param  robot  The robot that is executing the action.
 * @param  level  The level on which the action takes place.
 * @return        1 if the action was successful.
 */
static int take (Action *action, Robot *robot, Level *level)
{
    int xr, /* x coordinate of robot */
	yr, /* y coordinate of robot */
	xi, /* x coordinate of item */
	yi; /* y coordinate of item */

    /* work out coordinates */
    xr = robot->x;
    yr = robot->y;
    xi = xr + xoffset[robot->facing];
    yi = yr + yoffset[robot->facing];

    /* reject invalid action */
    if (xi < 0 || xi > 15 || yi < 0 || yi > 11)
	return 0; /* robot is looking off the map */
    if (! robot->hasinventory)
	return 0; /* an inventory is required to take items */
    if (level->items[xr + 16 * yr])
	return 0; /* robot already has an item */
    if (! level->items[xi + 16 * yi])
	return 0; /* there is nothing to take */

    /* take the item */
    level->items[xr + 16 * yr] = level->items[xi + 16 * yi];
    level->items[xi + 16 * yi] = NULL;
    return 1;
}

/**
 * Execute a Drop Item action.
 * @param  action The action to execute.
 * @param  robot  The robot that is executing the action.
 * @param  level  The level on which the action takes place.
 * @return        1 if the action was successful.
 */
static int drop (Action *action, Robot *robot, Level *level)
{
    int xr, /* x coordinate of robot */
	yr, /* y coordinate of robot */
	xi, /* x coordinate of item */
	yi; /* y coordinate of item */

    /* work out coordinates */
    xr = robot->x;
    yr = robot->y;
    xi = xr + xoffset[robot->facing];
    yi = yr + yoffset[robot->facing];

    /* reject invalid action */
    if (xi < 0 || xi > 15 || yi < 0 || yi > 11)
	return 0; /* robot is looking off the map */
    if (! robot->hasinventory)
	return 0; /* an inventory is required to drop items */
    if (! level->items[xr + 16 * yr])
	return 0; /* robot has no item to drop */
    if (level->items[xi + 16 * yi])
	return 0; /* an item is already there */
    if (! level->cells[xi + 16 * yi]->allowput)
	return 0; /* can't drop anything there */

    /* drop the item */
    level->items[xi + 16 * yi] = level->items[xr + 16 * yr];
    level->items[xr + 16 * yr] = NULL;
    return 1;
}

/**
 * Execute a Shoot action.
 * @param  action The action to execute.
 * @param  robot  The robot that is executing the action.
 * @param  level  The level on which the action takes place.
 * @return        1 if the action was successful.
 */
static int shoot (Action *action, Robot *robot, Level *level)
{
    int xr, /* x coordinate of robot */
	yr, /* y coordinate of robot */
	xf, /* x facing of robot */
	yf, /* y facing of robot */
	x, /* x coordinate counter */
	y, /* y coordinate counter */
	hit = 0; /* something is hit */
    Cell *cell; /* pointer to the current cell */

    /* work out coordinates */
    xr = robot->x;
    yr = robot->y;
    xf = xoffset[robot->facing];
    yf = yoffset[robot->facing];
    x = xr + xf;
    y = yr + yf;

    /* reject invalid action */
    if (! robot->hasphaser &&
	(! level->items[xr + 16 * yr] ||
	 level->items[xr + 16 * yr]->type != ITEM_PHASER))
	return 0; /* need a phaser to shoot */
    if (x < 0 || x > 15 || y < 0 || y > 11)
	return 0; /* robot is looking off the map */

    /* work out the shooting */
    do {

	/* record cell type in case it reacts to shooting */
	cell = level->cells[x + 16 * y];

	/* have we hit a robot? */
	if (level->robots[x + 16 * y]) {
	    level->robots[x + 16 * y]->status = ROBOT_DESTROYED;
	    hit = 1;
	}

	/* have we hit an item? */
	else if (level->items[x + 16 * y]) {
	    level->items[x + 16 * y]->status = ITEM_DESTROYED;
	    hit = 1;
	}

	/* have we hit a shootable block? */
	else if (cell->allowshoot) {
	    cell->onshoot (level, x, y);
	    hit = 1;
	}

	/* look at the next square */
	x += xf;
	y += yf;
    } while (x >=0 && x <= 15 && x >= 0 && y <= 11 && ! hit);

    /* tell the callling process we did shooting */
    return 1;
}

/*----------------------------------------------------------------------
 * Top Level Function Definitions.
 */

/**
 * Construct new action.
 * @param  type The type of action to construct.
 * @return      The new action.
 */
Action *get_Action (int type)
{
    Action *action; /* the new action */

    /* validate type */
    if (type < 1 || type > 12)
	return NULL;

    /* see if we have already created this action */
    if (actions[type - 1])
	return actions[type - 1];

    /* reserve memory for the action */
    if (! (action = malloc (sizeof (Action))))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* initialise attributes */
    action->type = type;
    action->name = names[type - 1];

    /* initialise methods */
    switch (type) {
    case ACTION_STEPFORWARD: action->execute = stepforward; break;
    case ACTION_STEPBACKWARD: action->execute = stepbackward; break;
    case ACTION_STEPLEFT: action->execute = stepleft; break;
    case ACTION_STEPRIGHT: action->execute = stepright; break;
    case ACTION_SPRINT: action->execute = sprint; break;
    case ACTION_LEAP: action->execute = leap; break;
    case ACTION_TURNLEFT: action->execute = turnleft; break;
    case ACTION_TURNRIGHT: action->execute = turnright; break;
    case ACTION_TURNABOUT: action->execute = turnabout; break;
    case ACTION_TAKE: action->execute = take; break;
    case ACTION_DROP: action->execute = drop; break;
    case ACTION_SHOOT: action->execute = shoot; break;
    }

    /* return the action */
    return actions[type - 1] = action;
}

/**
 * Destroy all actions.
 */
void destroy_Actions (void)
{
    int c; /* general counter */
    for (c = 0; c < 12; ++c)
	if (actions[c])
	    free (actions[c]);
}
