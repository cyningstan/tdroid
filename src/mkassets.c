/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Asset Creation Utility.
 */

/*----------------------------------------------------------------------
 * Headers
 */

/* ANSI C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <conio.h>
#include <dos.h>

/* project-specific headers */
#include "cgalib.h"
#include "speaker.h"
#include "robot.h"
#include "action.h"
#include "fatal.h"


/*----------------------------------------------------------------------
 * Data Definitions.
 */

/** @var output The output file handle. */
static FILE *output;

/** @var scr The screen. */
static Screen *scr;

/**
 * var playerrobotnames
 * The name of the six player robots
 */
static char *playerrobotnames[6] = {
    "Strider",
    "Bouncer",
    "Soldier",
    "Carrier",
    "Thinker",
    "Multibot"
};

/**
 * var playerrobotstats
 * The stats of the six player robots
 */
static int playerrobotstats[6][7] = {
    {1, 5, 1, 0, 0, 1, ACTION_STEPFORWARD},
    {2, 6, 0, 1, 0, 1, ACTION_LEAP},
    {3, 6, 0, 0, 1, 1, ACTION_SHOOT},
    {4, 7, 0, 0, 0, 1, ACTION_TAKE},
    {5, 8, 0, 0, 0, 1, ACTION_NONE},
    {6, 4, 1, 1, 1, 0, ACTION_NONE}
};

/**
 * var guardrobotstats
 * The stats of the guard robot.
 */
static int guardrobotstats[7] = {7, 8, 1, 1, 1, 0, ACTION_NONE};

/*----------------------------------------------------------------------
 * Level 3 Routines.
 */

/**
 * Extract a sprite (a bitmap and mask pair) from the raw assets.
 * @param x The X coordinate of the pair in the raw assets.
 * @param y The Y coordinate of the pair in the raw assets.
 */
void extractsprite (int x, int y)
{
    Bitmap *bit; /* bitmap or mask */
    if (! (bit = bit_create (16, 16)))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
    scr_get (scr, bit, x, y);
    bit_write (bit, output);
    scr_get (scr, bit, x + 16, y);
    bit_write (bit, output);
    bit_destroy (bit);
}

void extracttile (int x, int y)
{
    Bitmap *tile; /* the tile bitmap */
    if (! (tile = bit_create (16, 16)))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
    scr_get (scr, tile, x, y);
    bit_write (tile, output);
    bit_destroy (tile);
}

/*----------------------------------------------------------------------
 * Level 2 Routines.
 */

/**
 * Initialise the screen.
 */
static void initialise_screen (void)
{
    if (! (scr = scr_create (5)))
	fatalerror (FATAL_DISPLAY, __FILE__, __LINE__);
    //scr_palette (scr, 5, 3);
}

/**
 * Load raw assets from the PIC files.
 * @param filenum Which asset file, 0..2.
 */
void loadrawassets (int filenum)
{
    FILE *input; /* input file handle */
    char header[7], /* header from input file */
	*pic, /* pointer to picture buffer */
	filename[80]; /* name of input file */

    /* attempt to reserve memory for the picture */
    if (! (pic = malloc (16192)))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* attempt to open the input file and read the header */
    sprintf (filename, "assets/tdroid%d.pic", filenum);
    if (! (input = fopen (filename, "rb"))) {
	scr_destroy (scr);
	fatalerror (FATAL_NODATA, __FILE__, __LINE__);
    }
    if (! fread (header, 7, 1, input)) {
	scr_destroy (scr);
	fatalerror (FATAL_NODATA, __FILE__, __LINE__);
    }

    /* load the picture */
    if (! (fread (pic, 16192, 1, input))) {
	scr_destroy (scr);
	fatalerror (FATAL_NODATA, __FILE__, __LINE__);
    }

    /* put the picture on the screen and free the memory */
    _fmemcpy ((void far *) 0xb8000000, pic, 16192);
    free (pic);
}

/**
 * Extract the logo bitmap from the raw assets and save it.
 */
void makelogo (void)
{
    Bitmap *logo; /* the logo bitmap */
    if (! (logo = bit_create (128, 16)))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
    scr_get (scr, logo, 188, 180);
    bit_write (logo, output);
    bit_destroy (logo);
}

/**
 * Extract the title screen from the raw assets and save it.
 */
void maketitlescreen (void)
{
    Bitmap *title; /* the title screen bitmap */
    if (! (title = bit_create (320, 200)))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
    scr_get (scr, title, 0, 0);
    bit_write (title, output);
    bit_destroy (title);
}

/**
 * Extract the map border from the raw assets and save it.
 */
void makemapborder (void)
{
    Bitmap *border;
    if (! (border = bit_create (260, 200)))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
    scr_get (scr, border, 60, 0);
    bit_ink (border, 0);
    bit_box (border, 0, 4, 256, 192);
    bit_write (border, output);
    bit_destroy (border);
}

/**
 * Extract a panel from the raw assets and save it.
 * @param panelnum The panel to load.
 */
void makepanel (int panelnum)
{
    Bitmap *panel; /* single bitmap for each panel in turn */
    if (! (panel = bit_create (60, 200)))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
    if (panelnum == 0)
	scr_get (scr, panel, 0, 0);
    else
	scr_get (scr, panel, 60 * (panelnum - 1), 0);
    bit_write (panel, output);
    bit_destroy (panel);
}

/**
 * Extract a player robot from the raw assets and keep it for later.
 * @param robotnum The robot number 0..5.
 */
void makeplayerrobot (int robotnum)
{
    int f, /* facing number count */
	x, /* x coordinate of image on raw assets */
	y; /* y coordinate of image in raw assets */

    /* where are the robot images? */
    x = 92 + 32 * (robotnum / 3);
    y = 4 + 64 * (robotnum % 3);

    /* extract each face in turn */
    for (f = 0; f < 4; ++f)
	extractsprite (x, y + 16 * f);
}

/**
 * Extract the guard robot from the raw assets.
 */
void makeguardrobot (void)
{
    int f; /* facing number count */
    for (f = 0; f < 4; ++f)
	extractsprite (156, 132 + 16 * f);
}

/**
 * Extract the items from the raw assets.
 * @param type The item type 0..5.
 */
void makeitem (int type)
{
    extractsprite (156, 4 + 16 * type);
}

/**
 * Extract the cursor sprite from the raw assets.
 */
void makecursor (void)
{
    extractsprite (156, 100);
}

/**
 * Extract the explosion sprite from the raw assets.
 */
void makeexplosionsprite (void)
{
    extractsprite (156, 116);
}

/**
 * Extract the bump sprite from the raw assets.
 */
void makebumpsprite (void)
{
    extractsprite (188, 4);
}

/**
 * Extract a map tile from the raw assets.
 * @param tilenum The cell tile ID 0..11.
 */
void makemaptile (int tilenum)
{
    extracttile (60, 4 + 16 * tilenum);
}

/**
 * Extract an action tile from the raw assets.
 * @param tilenum The action tile ID 0..11.
 */
void makeactiontile (int tilenum)
{
    extracttile (76, 4 + 16 * tilenum);
}

/**
 * Extract a phaser tile from the raw assets.
 * @param facing the facing: 0 vertical, 1 horizontal.
 */
void makephasersprite (int facing)
{
    extractsprite (188, 20 + 16 * facing);
}

/**
 * Extract the RAM background tiles from the raw assets.
 * @param tilenum The RAM tile ID 0..4.
 */
void makeramtile (int tilenum)
{
    extracttile (188, 52 + 16 * tilenum);
}

/**
 * Extract the dialog box bitmap from the raw assets and save it.
 */
void makedialogbox (void)
{
    Bitmap *dialog; /* the dialog bitmap */
    if (! (dialog = bit_create (160, 32)))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
    scr_get (scr, dialog, 120, 0);
    bit_write (dialog, output);
    bit_destroy (dialog);
}

/**
 * Extract the progress bar components from the raw assets and save.
 */
void makeprogressbar (void)
{
    int c; /* general counter */
    Bitmap *section; /* section of progress bar */
    if (! (section = bit_create (4, 8)))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
    for (c = 0; c < 4; ++c) {
	scr_get (scr, section, 188 + 4 * c, 172);
	bit_write (section, output);
    }
    bit_destroy (section);
}

/**
 * Close the screen.
 */
static void close_screen (void)
{
    scr_destroy (scr);
}

/**
 * Generate and save the data for a robot.
 * @param name The name of the robots.
 * @param stats A pointer to the stats array.
 */
static void makerobotdata (char *name, int *stats)
{
    Robot *robot; /* temporary robot object */

    /* reserve memory for robot */
    if (! (robot = new_Robot (stats[0])))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* copy name */
    strcpy (robot->name, name);

    /* copy stats */
    robot->type = stats[0];
    robot->ramsize = stats[1];
    robot->haswalker = stats[2];
    robot->hasspring = stats[3];
    robot->hasphaser = stats[4];
    robot->hasinventory = stats[5];
    robot->rom = stats[6];
    memset (robot->ram, '\0', 8);
    robot->x = 0xff;
    robot->y = 0xff;

    /* write the robot */
    robot->write (robot, output);
}

/*----------------------------------------------------------------------
 * Level 1 Routines.
 */

/**
 * Create the output file.
 */
void createoutputfile (void)
{
    if (! (output = fopen ("tdroid/tdroid.dat", "wb")))
	fatalerror (FATAL_NODATA, __FILE__, __LINE__);
    if (! (fwrite ("TDR100D", 8, 1, output)))
	fatalerror (FATAL_NODATA, __FILE__, __LINE__);
}

/**
 * Create the graphical assets.
 */
void makegraphicalassets (void)
{
    int c; /* general purpose counter */
    initialise_screen ();
    loadrawassets (0);
    makelogo ();
    loadrawassets (2);
    maketitlescreen ();
    loadrawassets (0);
    makemapborder ();
    for (c = 0; c < 3; ++c) {
	if (c)
	    loadrawassets (1);
	else
	    loadrawassets (0);
	makepanel (c);
    }
    loadrawassets (0);
    for (c = 0; c < 6; ++c)
	makeplayerrobot (c);
    makeguardrobot ();
    for (c = 0; c < 6; ++c)
	makeitem (c);
    makecursor ();
    makeexplosionsprite ();
    makebumpsprite ();
    for (c = 0; c < 12; ++c)
	makemaptile (c);
    for (c = 0; c < 12; ++c)
	makeactiontile (c);
    for (c = 0; c < 2; ++c)
	makephasersprite (c);
    for (c = 0; c < 5; ++c)
	makeramtile (c);
    makeprogressbar ();
    loadrawassets (1);
    makedialogbox ();
    close_screen ();
}

/**
 * Create the font assets.
 */
void makefontassets (void)
{
    /* local variables */
    FILE *input; /* input file handle */
    char header[8]; /* header read from input file */
    Font *fnt; /* font read from input file */

    /* open input file, read and verify header */
    if (! (input = fopen ("cgalib/cgalib/fnt/future.fnt", "rb")))
	fatalerror (FATAL_NODATA, __FILE__, __LINE__);
    if (! (fread (header, 8, 1, input)))
	fatalerror (FATAL_NODATA, __FILE__, __LINE__);
    if (strcmp (header, "CGA100F"))
	fatalerror (FATAL_NODATA, __FILE__, __LINE__);

    /* read the font and copy it to the output file */
    if (! (fnt = fnt_read (input)))
	fatalerror (FATAL_NODATA, __FILE__, __LINE__);
    fclose (input);
    fnt_write (fnt, output);
    fnt_destroy (fnt);
}

/**
 * Create the sound assets.
 */
void makesoundassets (void)
{
    Effect *effect; /* a sound effect object */

    /* create the sound objects */
    if (! (effect = new_Effect ()))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* robot deployment noise */
    effect->pattern = EFFECT_FALL;
    effect->repetitions = 1;
    effect->low = 36;
    effect->high = 48;
    effect->duration = 2;
    effect->pause = 0;
    effect->write (effect, output);

    /* generate move beat */
    effect->pattern = EFFECT_NOISE;
    effect->repetitions = 1;
    effect->low = 24;
    effect->high = 24;
    effect->duration = 1;
    effect->pause = 0;
    effect->write (effect, output);

    /* generate and save the pew-pew noise */
    effect->pattern = EFFECT_FALL;
    effect->repetitions = 2;
    effect->low = 0;
    effect->high = 60;
    effect->duration = 3;
    effect->pause = 0;
    effect->write (effect, output);

    /* generate and save the blast noise */
    effect->pattern = EFFECT_NOISE;
    effect->repetitions = 1;
    effect->low = 12;
    effect->high = 36;
    effect->duration = 3;
    effect->pause = 0;
    effect->write (effect, output);

    /* generate and save the forcefield deactivation noise */
    effect->pattern = EFFECT_FALL;
    effect->repetitions = 1;
    effect->low = 12;
    effect->high = 48;
    effect->duration = 6;
    effect->pause = 0;
    effect->write (effect, output);

    /* generate and save the teleport noise */
    effect->pattern = EFFECT_RISE;
    effect->repetitions = 4;
    effect->low = 36;
    effect->high = 48;
    effect->duration = 2;
    effect->pause = 0;
    effect->write (effect, output);

    /* generate and save the level completed noise */
    effect->pattern = EFFECT_RISE;
    effect->repetitions = 1;
    effect->low = 36;
    effect->high = 60;
    effect->duration = 6;
    effect->pause = 0;
    effect->write (effect, output);

    /* generate and save the level failed noise */
    effect->pattern = EFFECT_FALL;
    effect->repetitions = 1;
    effect->low = 36;
    effect->high = 60;
    effect->duration = 6;
    effect->pause = 0;
    effect->write (effect, output);

    /* generate and save the level completed noise */
    effect->pattern = EFFECT_RISE;
    effect->repetitions = 3;
    effect->low = 36;
    effect->high = 60;
    effect->duration = 3;
    effect->pause = 0;
    effect->write (effect, output);

    /* clean up */
    effect->destroy (effect);
}

/**
 * Create the data assets.
 */
void makedataassets (void)
{
    int c; /* general purpose counter */
    for (c = 0; c < 6; ++c)
	makerobotdata (playerrobotnames[c], playerrobotstats[c]);
    makerobotdata ("Guard", guardrobotstats);
}

/**
 * Close the output file.
 */
void closeoutputfile (void)
{
    fclose (output);
}

/*----------------------------------------------------------------------
 * Top Level Routine.
 */

/**
 * Main program.
 * @return 0 if successful.
 */
int main (void)
{
    createoutputfile ();
    makegraphicalassets ();
    makefontassets ();
    makesoundassets ();
    makedataassets ();
    closeoutputfile ();
    return 0;
}
