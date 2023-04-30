/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2020.
 * Init File Hander Module.
 */

/*----------------------------------------------------------------------
 * Headers.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* project-specific headers */
#include "fatal.h"
#include "config.h"
#include "utils.h"

/*----------------------------------------------------------------------
 * File Level Variables.
 */

/** @var config The configuration - there should be only one. */
static Config *config = NULL;

/*----------------------------------------------------------------------
 * Public Methods.
 */

/**
 * Destroy the configuration when no longer needed.
 */
static void destroy (void)
{
    if (config) {
        free (config);
        config = NULL;
    }
}

/**
 * Load the configuration from a file.
 */
static void load (void)
{
    /* local variables */
    FILE *input; /* init file handle */
    char header[8]; /* header read from init file */

    /* if there's an input file, load the values from it */
    if ((input = fopen ("tdroid.ini", "rb"))) {

        /* read and verify header */
        if (! fread (header, 8, 1, input))
            fatalerror (FATAL_INVALIDINIT, __FILE__, __LINE__);
        if (strcmp (header, "TDR100I"))
            fatalerror (FATAL_INVALIDINIT, __FILE__, __LINE__);

        /* read the data proper */
        if (! readstring (config->levelpackfile, input))
            fatalerror (FATAL_INVALIDINIT, __FILE__, __LINE__);
	if (! readstring (config->gamefile, input))
	    fatalerror (FATAL_INVALIDINIT, __FILE__, __LINE__);
	if (! readstring (config->player, input))
	    fatalerror (FATAL_INVALIDINIT, __FILE__, __LINE__);

        /* close the file */
        fclose (input);
    }
}

/**
 * Save values to the initialisation file and clean up.
 */
static void save (void)
{
    /* local variables */
    FILE *output; /* init file handle */

    /* attempt to open the output file */
    if ((output = fopen ("tdroid.ini", "wb"))) {

	/* write header */
	fwrite ("TDR100I", 8, 1, output);

	/* write the data proper */
	writestring (config->levelpackfile, output);
	writestring (config->gamefile, output);
	writestring (config->player, output);

	/* close the file */
	fclose (output);
    }
}

/*----------------------------------------------------------------------
 * Top Level Function Definitions.
 */

/**
 * Construct a new configuration.
 * @return The new configuration.
 */
Config *new_Config (void)
{
    /* reserve memory for configuration */
    if (config)
	return config;
    else if (! (config = malloc (sizeof (Config))))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* initialise methods */
    config->destroy = destroy;
    config->load = load;
    config->save = save;

    /* initialise properties */
    strcpy (config->levelpackfile, "TDROID.LEV");
    *config->gamefile = '\0';
    strcpy (config->player, "Cyningstan");

    /* return the configuration */
    return config;
}
