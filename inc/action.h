/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Action Header.
 */

/* Structures defined in this header */
typedef struct action Action;

#ifndef __ACTION_H__
#define __ACTION_H__

/*----------------------------------------------------------------------
 * Required Headers.
 */

/* project specific headers */
#include "robot.h"
#include "level.h"

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/**
 * @enum ActionType
 * Named action constants.
 */
typedef enum {
    ACTION_NONE, /* no action */
    ACTION_STEPFORWARD, /* step forward 1 space */
    ACTION_STEPBACKWARD, /* step backward 1 space */
    ACTION_STEPLEFT, /* step left 1 space - requires walker */
    ACTION_STEPRIGHT, /* step right 1 space - requres walker */
    ACTION_SPRINT, /* sprint forward 2 spaces */
    ACTION_LEAP, /* leap forward 2 spaces - requires spring */
    ACTION_TURNLEFT, /* turn 90 degrees left */
    ACTION_TURNRIGHT, /* turn 90 degrees right */
    ACTION_TURNABOUT, /* turn 180 degrees */
    ACTION_TAKE, /* take an item - requires inventory */
    ACTION_DROP, /* drop an item - requires inventory */
    ACTION_SHOOT /* shoot - requires phaser */
} ActionType;

/**
 * @enum ActionStatus
 * Status of an action in a robot's RAM.
 */
typedef enum {
    ACTION_NOTDONE,
    ACTION_CANCELLED,
    ACTION_DONE
} ActionStatus;

/**
 * @struct action
 * An action object.
 */
struct action {

    /*
     * Attributes
     */

    /* @var type The action type */
    int type;

    /* @var name The name of the action */
    char *name; /* points to a static array */

    /*
     * Methods
     */

    /**
     * Execute the action.
     * @param  action     The action to execute.
     * @param  robot      The robot that is executing the action.
     * @param  level      The level on which the action takes place.
     * @param  effecthook The function to log cell effects
     * @return            1 if the action was successful.
     */
    int (*execute) (Action *action, Robot *robot, Level *level);

};

/*----------------------------------------------------------------------
 * Top Level Function Declarations.
 */

/**
 * Return an action by type.
 * @param  type The type of action to construct.
 * @return      The new action.
 */
Action *get_Action (int type);

/**
 * Destroy all actions.
 */
void destroy_Actions (void);

#endif
