/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Main Header.
 */

#ifndef __TDROID_H__
#define __TDROID_H__

/* required headers */
#include "controls.h"
#include "display.h"
#include "config.h"

/*----------------------------------------------------------------------
 * Top Level Function Declarations.
 */

/**
 * Share the control handler with other modules.
 * @return A pointer to the game control handler.
 */
Controls *getcontrols (void);

/**
 * Share the display handler with other modules.
 * @return A pointer to the display module.
 */
Display *getdisplay (void);

/**
 * Share the config handler with other modules.
 * @return A pointer to the config module.
 */
Config *getconfig (void);

/**
 * Load the robots from the already-open asset file.
 * @param input The asset file handle.
 */
void loadrobotdetails (FILE *input);

/**
 * Create a new player robot and populate its details.
 * @param  type The type of robot.
 * @return      The player robot.
 */
Robot *new_PlayerRobot (int type);

#endif
