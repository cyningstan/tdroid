/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker 2022.
 * Map cell header.
 */

/* types defined in this file */
typedef struct cell Cell;

#ifndef __CELL_H__
#define __CELL_H__

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/* required headers */
#include <stdio.h>
#include "level.h"
#include "robot.h"

/**
 * @enum CellTypes
 * Enumerated constants for all the cell types.
 */
typedef enum {
    CELL_NONE, /* cell is undefined */
    CELL_FLOOR, /* open floor */
    CELL_WALL, /* impenetrable wall */
    CELL_CONVEYOR_NORTH, /* north-moving conveyor belt */
    CELL_CONVEYOR_EAST, /* east-moving conveyor belt */
    CELL_CONVEYOR_SOUTH, /* south-moving conveyor belt */
    CELL_CONVEYOR_WEST, /* west-moving conveyor belt */
    CELL_TURNTABLE_CLOCKWISE, /* clockwise turntable */
    CELL_TURNTABLE_ANTICLOCKWISE, /* anti-clockwise turntable */
    CELL_TELEPORTER, /* teleporter */
    CELL_FORCEFIELD, /* deadly forcefield */
    CELL_GENERATOR, /* forcefield generator */
    CELL_READER, /* card reader */
    CELL_LAST /* placeholder */
} CellType;

/**
 * @enum CellEffectTypes
 * Enumerated constants for the possible cell effects.
 */
typedef enum {
    CELL_NOEFFECT, /* no effect - should not be used */
    CELL_MOVE, /* a robot has moved on to the cell */
    CELL_PUT, /* an item has been dropped or pushed on to the cell */
    CELL_GET, /* an item has been taken from or pushed off the cell */
    CELL_SHOOT /* an cell has been shot */
} CellEffectType;

/**
 * @struct cell
 * Model of a single cell on the map.
 */
struct cell {

    /*
     * Attributes
     */

    /** @var type The cell ID for use in saved levels and games. */
    int type;

    /** @var name The cell's name. */
    char *name;

    /** @var allowmove 1 if robots can move onto here, 0 otherwise. */
    int allowmove;

    /** @var allowput 1 if items can be dropped or placed, else 0. */
    int allowput;

    /** @var allowshoot 1 if this cell can be shot, 0 otherwise. */
    int allowshoot;

    /*
     * Methods
     */

    /**
     * The effect when a robot sits here.
     * @param level The level map.
     * @param x     The x coordinate of the cell.
     * @param y     The y coordinate of the cell.
     */
    int (*onrobot) (Level *level, int x, int y);

    /**
     * The effect when an item sits here.
     * @param level The level map.
     * @param x     The x coordinate of the cell.
     * @param y     The y coordinate of the cell.
     */
    int (*onitem) (Level *level, int x, int y);

    /**
     * The effect when the cell is shot.
     * @param level The level map.
     * @param x     The x coordinate of the cell.
     * @param y     The y coordinate of the cell.
     */
    int (*onshoot) (Level *level, int x, int y);

};

/*----------------------------------------------------------------------
 * Top-level Function Declarations.
 */

/**
 * Cell contstructor function.
 * @param  type The cell type 1..12.
 * @return      The new cell.
 */
Cell *get_Cell (int type);

/**
 * Destroy all cells.
 */
void destroy_Cells (void);

#endif
