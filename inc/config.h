/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Init File Hander Header.
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

/* project includes */
#include "uiscreen.h"

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/** @struct config The configuration. */
typedef struct config Config;
struct config {

    /** @var campaignfile Default campaign file for a new game. */
    char levelpackfile[13];

    /** @var gamefile The filename for the game in play. */
    char gamefile[13];

    /** @var player The last player to play. */
    char player[14];

    /*
     * Public Method Declarations.
     */

    /**
     * Destroy the configuration when no longer needed.
     */
    void (*destroy) (void);

    /**
     * Load the configuration from a file.
     */
    void (*load) (void);

    /**
     * Save the configuration to a file.
     */
    void (*save) (void);

};

/*----------------------------------------------------------------------
 * Top Level Function Declarations.
 */

/**
 * Construct a new configuration.
 * @return The new configuration.
 */
Config *new_Config (void);

#endif
