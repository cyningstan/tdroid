/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker 2022.
 * Robot header.
 */

/* types defined in this file */
typedef struct robot Robot;

#ifndef __ROBOT_H__
#define __ROBOT_H__

/*----------------------------------------------------------------------
 * Required Headers.
 */

/* standard C headers */
#include <stdio.h>

/* project specific headers */
#include "cgalib.h"
#include "level.h"
#include "action.h"

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/**
 * @enum RobotType
 * Identifiers for the robot types.
 */
typedef enum {
    ROBOT_NONE,
    ROBOT_STRIDER,
    ROBOT_BOUNCER,
    ROBOT_SOLDIER,
    ROBOT_CARRIER,
    ROBOT_THINKER,
    ROBOT_MULTIBOT,
    ROBOT_GUARD
} RobotType;

/**
 * @enum RobotFacing
 * Identifiers for the robot directions.
 */
typedef enum {
    ROBOT_NORTH,
    ROBOT_EAST,
    ROBOT_SOUTH,
    ROBOT_WEST
} RobotFacing;

/**
 * @enum RobotStatus
 * Identifiers for the robot status.
 */
typedef enum {
    ROBOT_INERT, /* nothing has happened to the robot */
    ROBOT_CONVEYED, /* the robot has been conveyed this move */
    ROBOT_TELEPORTED, /* the robot has been teleported this move */
    ROBOT_DESTROYED /* the robot has been destroyed this move */
} RobotStatus;

/**
 * @struct robot
 * Attributes and methods to model a robot.
 */
struct robot {

    /*
     * Attributes
     */

    /** @var name The robot's name. */
    char name[9];

    /** @var type The type of robot. */
    int type;

    /** @var ramsize The number of actions a robot can take in a turn. */
    int ramsize;

    /** @var haswalker 1 if the robot has a built-in walker, 0 if not. */
    int haswalker;

    /** @var hasspring 1 if the robot has a built-in spring, 0 if not. */
    int hasspring;

    /** @var hasphaser 1 if the robot has a built-in phaser, 0 if not. */
    int hasphaser;

    /** @var hasinventory 1 if the robot has an inventory, 0 if not. */
    int hasinventory;

    /** @var rom The action stored in the robot's ROM. */
    int rom;

    /** @var ram The actions stored in the robot's RAM. */
    int ram[8];

    /** @var x The current X coordinate of the robot. */
    int x;

    /** @var y The current Y coordinate of the robot. */
    int y;

    /**
     * @var facing
     * The current facing of the robot:
     * 0 = north, 1 = east, 2 = south, 3 = west.
     */
    int facing;

    /**
     * @var status
     * The robot status.
     */
    int status;

    /*
     * Methods
     */

    /**
     * Destroy the robot when no longer needed.
     * @param robot The robot to destroy.
     */
    void (*destroy) (Robot *robot);

    /**
     * Clone the robot.
     * @param  robot The robot to clone.
     * @return       The new robot.
     */
    Robot *(*clone) (Robot *robot);

    /**
     * Clear the robot data.
     * @param robot The robot object to clear.
     */
    void (*clear) (Robot *robot);

    /**
     * Write the robot to an already open file.
     * @param robot The robot to write.
     * @param output The output file handle.
     * @return 1 if successful, 0 on failure.
     */
    int (*write) (Robot *robot, FILE *output);

    /**
     * Read the robot from an already open file.
     * @param robot The robot to read.
     * @param input The input file handle.
     * @return 1 if successful, 0 on failure.
     */
    int (*read) (Robot *robot, FILE *output);

    /**
     * Perform an action.
     * @param robot  The robot to act.
     * @param level  The level on which the action is performed.
     * @param action The action to perform.
     */
    void (*act) (Robot *robot, Level *level, Action *action);

};

/*----------------------------------------------------------------------
 * Top-level Function Declarations.
 */

/**
 * Robot contstructor function.
 * @param  type The robot type 0 (for guard) or 1..6.
 * @return The new robot.
 */
Robot *new_Robot (int type);

#endif
