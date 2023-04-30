/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Level Pack Module.
 */

/*----------------------------------------------------------------------
 * Headers.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* project specific headers */
#include "levelpak.h"
#include "level.h"
#include "scoretbl.h"
#include "utils.h"

/*----------------------------------------------------------------------
 * Public Method Function Definitions.
 */

/**
 * Destroy the level pack when no longer needed.
 * @param levelpack The pack to destroy.
 */
static void destroy (LevelPack *levelpack)
{
    int c; /* general counter */
    Level *level; /* pointer to a level */

    /* only destroy what exists */
    if (levelpack) {

	/* destroy the levels */
	for (c = 0; c < 12; ++c)
	    if ((level = levelpack->levels[c]))
		level->destroy (level);

	/* destroy the score entries */
	if (levelpack->scoretable)
	    levelpack->scoretable->destroy (levelpack->scoretable);

	/* destroy the level pack */
	free (levelpack);
    }
}

/**
 * Clear the level pack data.
 * @param levelpack The pack to clear.
 */
static void clear (LevelPack *levelpack)
{
    int c; /* counter */
    Level *level; /* pointer to a level */

    /* reset the name */
    *levelpack->name = '\0';

    /* clear out the levels */
    for (c = 0; c < 12; ++c) {
	level = levelpack->levels[c];
	level->clear (level);
    }

    /* clear the scores */
    levelpack->scoretable->clear (levelpack->scoretable);
}

/**
 * Save the level pack. Filename is in the attributes.
 * @param  levelpack The pack to save.
 * @return           1 if successful, 0 on failure.
 */
static int save (LevelPack *levelpack)
{
    FILE *output; /* the output file */
    int r = 1, /* return value */
	c; /* general purpose counter */
    Level *level; /* pointer to a level */

    /* open the output file and write the header */
    if (! (output = fopen (levelpack->filename, "wb")))
	return 0;
    r = r && fwrite ("TDR100L", 8, 1, output);

    /* write the name */
    r = r && writestring (levelpack->name, output);

    /* write the levels */
    for (c = 0; c < 12; ++c) {
	level = levelpack->levels[c];
	r = r && level->write (level, output);
    }

    /* close the file and return */
    fclose (output);
    return r;
}

/**
 * Save the high score table. Filename is in the attributes.
 * @param  levelpack The pack to save.
 * @return           1 if successful, 0 on failure.
 */
static int savescores (LevelPack *levelpack)
{
    FILE *output; /* the output file */
    char filename[13], /* the score table filename */
	*ptr; /* pointer to the '.' in the filename */
    int r = 1; /* return value */

    /* ascertain the score table filename */
    strcpy (filename, levelpack->filename);
    if ((ptr = strchr (filename, '.')))
	strcpy (ptr, ".sco");
    else
	strcat (filename, ".sco");

    /* save the score table */
    if (! (output = fopen (filename, "wb")))
	return 0;
    r = r && fwrite ("TDR100S", 8, 1, output);
    r = r &&
	levelpack->scoretable->write (levelpack->scoretable, output);
    fclose (output);
    return r;
}

/**
 * Load the level pack. Filename is in the attributes.
 * @param  levelpack The pack to load.
 * @param  summary   1 if only a summary is to be loaded.
 * @return           1 if successful, 0 on failure.
 */
static int load (LevelPack *levelpack, int summary)
{
    FILE *input; /* the input file */
    char header[8], /* header read in from file */
	filename[13], /* the score table filename */
	*ptr; /* pointer to the '.' in the filename */
    int r = 1, /* return value */
	c; /* general counter */
    Level *level; /* pointer to a level */

    /* open the input file and read the header */
    if (! (input = fopen (levelpack->filename, "rb")))
	return 0;
    r = r && fread (header, 8, 1, input);
    r = r && ! strcmp (header, "TDR100L");

    /* read the name */
    r = r && readstring (levelpack->name, input);

    /* return now if only a summary is needed */
    if (summary) {
	fclose (input);
	return r;
    }

    /* read the levels */
    for (c = 0; c < 12; ++c) {
	level = levelpack->levels[c];
	r = r && level->read (level, input);
    }

    /* close the level pack file */
    fclose (input);
    if (! r)
	return r;

    /* ascertain the score table filename */
    strcpy (filename, levelpack->filename);
    if ((ptr = strchr (filename, '.')))
	strcpy (ptr, ".sco");
    else
	strcat (filename, ".sco");

    /* open the score table file if present */
    if (! (input = fopen (filename, "rb")))
	return r;
    if (! (fread (header, 8, 1, input))) {
	fclose (input);
	return r;
    }
    if (strcmp (header, "TDR100S")) {
	fclose (input);
	return r;
    }

    /* read the scores and return */
    levelpack->scoretable->read (levelpack->scoretable, input);
    fclose (input);
    return r;
}

/*----------------------------------------------------------------------
 * Top Level Function Definitions.
 */

/**
 * Create a new level pack.
 * @return The new level pack.
 */
LevelPack *new_LevelPack (void)
{
    LevelPack *levelpack; /* the new level pack */
    int c; /* general counter */

    /* reserve memory for the level pack */
    if (! (levelpack = malloc (sizeof (LevelPack))))
	return NULL;

    /* initialise the attributes */
    if (! (levelpack->scoretable = new_ScoreTable ())) {
	free (levelpack);
	return NULL;
    }
    for (c = 0; c < 12; ++c)
	if (! (levelpack->levels[c] = new_Level ())) {
	    levelpack->scoretable->destroy (levelpack->scoretable);
	    free (levelpack);
	    return NULL;
	}

    /* initialise the methods */
    levelpack->destroy = destroy;
    levelpack->clear = clear;
    levelpack->save = save;
    levelpack->savescores = savescores;
    levelpack->load = load;

    /* return the new level pack */
    return levelpack;
}
