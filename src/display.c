/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Display Module.
 */

/*----------------------------------------------------------------------
 * Headers.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* compiler specific headers */
#include <conio.h>
#include <dos.h>

/* project-specific headers */
#include "tdroid.h"
#include "display.h"
#include "fatal.h"
#include "game.h"
#include "controls.h"
#include "timer.h"
#include "cgalib.h"
#include "speaker.h"


/*----------------------------------------------------------------------
 * Data Definitions.
 */

/**
 * @struct displaylist
 * A linked list containing coordinates to be updated.
 */
typedef struct displaylist DisplayList;
struct displaylist {
    int x, /* x coordinate to update */
	y, /* y coordinate to update */
	w, /* width to update */
	h; /* height to update */
    DisplayList *next; /* next entry in display list */
};

/**
 * @enum panelimageids are the IDs for the panel images.
 */
typedef enum {
    PANEL_BLANK, /* blank for when no panel is needed */
    PANEL_DEPLOY, /* robot deployment panel */
    PANEL_PROGRAM, /* panel for programming the robots */
    PANEL_LAST /* placeholder */
} PanelImageIDs;

/** @var display The display object */
static Display *display = NULL;

/** @var displaylist A list of areas to update. */
static DisplayList *displaylist = NULL;

/** @var screen is the CGALIB screen data. */
static Screen *screen;

/** @var controls A pointer to the game control handler. */
static Controls *controls;

/** @var scrbuf The screen buffer. */
static Bitmap *scrbuf;

/** @var title is the title screen image. */
static Bitmap *title;

/** @var panels are the game screen panel images. */
static Bitmap *panels[PANEL_LAST];

/** @var robots The robot bitmaps, including guards. */
static Bitmap *robots[7][4];

/** @var robotmasks The robot masks, including guards. */
static Bitmap *robotmasks[7][4];

/** @var items The item bitmaps */
static Bitmap *items[6];

/** @var itemmasks The item masks */
static Bitmap *itemmasks[6];

/** @var cursor is the map cursor. */
static Bitmap *cursor;

/** @var cursormask is the map cursor mask. */
static Bitmap *cursormask;

/** @var flash is the unit firing bitmap. */
static Bitmap *bump;

/** @var flashmask is the unit firing mask. */
static Bitmap *bumpmask;

/** @var blast is the unit exploding bitmap. */
static Bitmap *blast;

/** @var blastmask is the unit exploding mask. */
static Bitmap *blastmask;

/** @var back is the map border. */
static Bitmap *border;

/** @var maptiles The tiles for the level map. */
static Bitmap *maptiles[12];

/** @var actiontiles The tiles for the actions. */
static Bitmap *actiontiles[12];

/** @var phaserbeams The sprites for phaser beams. */
static Bitmap *phaserbeams[2];

/** @var phasermasks The masks for phaser beams. */
static Bitmap *phasermasks[2];

/** @var ramtiles The tiles for RAM, ROM and the inventory. */
static Bitmap *ramtiles[5];

/** @var dialoguebox The dialog box bitmap. */
static Bitmap *dialoguebox;

/** @var progressbar Sections of the action progress bar. */
static Bitmap *progressbar[4];

/** @var font is the font in standard colours. */
static Font *font;

/** @var highfont is the font in highlighted colours. */
static Font *highfont;

/** @var lowfont is the font in lowlighted colours. */
static Font *lowfont;

/** @var soundenabled 1 if sound enabled, 0 if not. */
static int soundenabled;

/** @var tune The intro tune. */
static Tune *tune = NULL;

/** @var noises A selection of sound effect noises. */
static Effect *noises[9];

/** @var scrtitle The title of the current screen. */
static char scrtitle[4][14];

/** @var game The game being played. */
static Game *game = NULL;

/** 
 * @var wholemap
 * If 1, then the whole map display is being updated,
 * so don't queue screen updates for each square.
 */
static int wholemap = 0;

/*----------------------------------------------------------------------
 * Service Level Private Functions.
 */

/**
 * Add a screen area to the display list (areas to be updated).
 * @param x The x coordinate of the area.
 * @param y The y coordinate of the area.
 * @param w The width of the area.
 * @param h The height of the area.
 */
static void queueupdate (int x, int y, int w, int h)
{
    DisplayList *entry; /* new entry for display list */

    /* reserve memory for new display list entry */
    if (! (entry = malloc (sizeof (DisplayList))))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* fill in the display list entry */
    entry->x = x;
    entry->y = y;
    entry->w = w;
    entry->h = h;

    /* add to the display list */
    entry->next = displaylist;
    displaylist = entry;
}

/**
 * Edit a short piece of text at an arbitrary position on the screen.
 * @param etext  The string buffer.
 * @param length The maximum string length.
 * @param x      X coordinate to display.
 * @param y      Y coordinate to display.
 */
static void edittext (char *etext, int length, int x, int y)
{
    int ascii; /* ascii code of key pressed */
    char buf[65]; /* display buffer */

    /* display the initial prompt and string */
    sprintf (buf, "%-*.*s", length, length, etext);
    bit_ink (scrbuf, 0);
    bit_box (scrbuf, x, y, 4 * length, 8);
    bit_ink (scrbuf, 3);
    bit_font (scrbuf, highfont);
    bit_print (scrbuf, x, y, buf);
    queueupdate (x, y, 4 * length, 8);
    display->update ();

    /* main edit loop */
    do {

	/* get a keypress */
	ascii = controls->key ();

	/* process an ASCII character */
	if (ascii >= ' ' && ascii <= '~' && strlen (etext) < length) {
	    if (! (ascii == ' ' && strlen (etext) == 0)) {
		etext[strlen (etext) + 1] = '\0';
		etext[strlen (etext)] = ascii;
		bit_print (scrbuf, x, y, etext);
		queueupdate (x, y, 4 * length, 8);
		display->update ();
	    } 
	}

	/* process backspace */
	else if (ascii == 8 && *etext) {
	    etext[strlen (etext) - 1] = '\0';
	    sprintf (buf, "%-*.*s", length, length, etext);
	    bit_print (scrbuf, x, y, buf);
	    bit_print (scrbuf, x + 4 * strlen (buf), y, " ");
	    queueupdate (x, y, 4 * length, 8);
	    display->update ();
	}
	
    } while (ascii != 13);

    /* clean up and wait for enter release */
    bit_ink (scrbuf, 3);
    bit_font (scrbuf, font);
    bit_print (scrbuf, x, y, buf);
    display->update ();
    while (controls->fire ());
}

/*----------------------------------------------------------------------
 * Level 1 Private Function Definitions.
 */

/**
 * Load the graphical assets.
 */
static void loadassets (void)
{
    FILE *input; /* input file */
    char header[8]; /* the input file header */
    Bitmap *logo; /* the Cyningstan logo */
    time_t start; /* time the Cyningstan logo was displayed */
    int c, /* general counter */
	f; /* robot facing counter */

    /* open input file */
    if (! (input = fopen ("tdroid.dat", "rb")))
	fatalerror (FATAL_NODATA, __FILE__, __LINE__);
    else if (! fread (header, 8, 1, input))
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    else if (strcmp (header, "TDR100D"))
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);

    /* load and display the Cyningstan logo */
    start = time (NULL);
    if (! (logo = bit_read (input)))
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    scr_put (screen, logo, 96, 92, DRAW_PSET);
    bit_destroy (logo);

    /* load the title screen graphic */
    if (! (title = bit_read (input)))
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);

    /* load the border and panel graphics */
    if (! (border = bit_read (input)))
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    for (c = 0; c < 3; ++c)
	if (! (panels[c] = bit_read (input)))
	    fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);

    /* read the robots */
    for (c = 0; c < 7; ++c)
	for (f = 0; f < 4; ++f) {
	    if (! (robots[c][f] = bit_read (input)))
		fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
	    if (! (robotmasks[c][f] = bit_read (input)))
		fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
	}

    /* read the items */
    for (c = 0; c < 6; ++c) {
	if (! (items[c] = bit_read (input)))
	    fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
	if (! (itemmasks[c] = bit_read (input)))
	    fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    }

    /* load various one-off sprites */
    if (! (cursor = bit_read (input)))
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    if (! (cursormask = bit_read (input)))
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    if (! (blast = bit_read (input)))
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    if (! (blastmask = bit_read (input)))
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    if (! (bump = bit_read (input)))
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    if (! (bumpmask = bit_read (input)))
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);

    /* load the map cell tiles */
    for (c = 0; c < 12; ++c)
	if (! (maptiles[c] = bit_read (input)))
	    fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);

    /* load the action tiles */
    for (c = 0; c < 12; ++c)
	if (! (actiontiles[c] = bit_read (input)))
	    fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);

    /* load the phaser sprites */
    for (c = 0; c < 2; ++c) {
	if (! (phaserbeams[c] = bit_read (input)))
	    fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
	if (! (phasermasks[c] = bit_read (input)))
	    fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    }

    /* load the RAM/ROM/Inventory filler tiles */
    for (c = 0; c < 5; ++c)
	if (! (ramtiles[c] = bit_read (input)))
	    fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);

    /* load the progress bar */
    for (c = 0; c < 4; ++c)
	if (! (progressbar[c] = bit_read (input)))
	    fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);

    /* load the dialog box border */
    if (! (dialoguebox = bit_read (input)))
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);

    /* load the font and initialise the screen with it */
    if (! (font = fnt_read (input)))
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    scr_font (screen, font);

    /* create the highlighted and lowlighted fonts */
    highfont = fnt_copy (font);
    fnt_colours (highfont, 3, 2);
    lowfont = fnt_copy (font);
    fnt_colours (lowfont, 1, 0);

    /* load the sound effects */
    for (c = 0; c < 9; ++c) {
	if (! (noises[c] = new_Effect ()))
	    fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
	if (! (noises[c]->read (noises[c], input)))
	    fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    }

    /* load the robot details */
    loadrobotdetails (input);
    
    /* clear the logo after at least three seconds */
    while (time (NULL) < start + 4);
    scr_ink (screen, 0);
    scr_box (screen, 96, 92, 128, 16);
    sleep (1);
}

/**
 * Centre a line of text.
 * @var dest The destination string.
 * @var src  The source string.
 * @var len  The length of the string.
 */
static void centreline (char *dest, char *src, int len)
{
    int srclen, /* length of source string */
	padlen; /* length of padding */

    /* return the source string as is if it fills the space */
    if (len == (srclen = strlen (src)))
	strcpy (dest, src);

    /* truncate the source string if it overfills the space */
    else if (len < srclen) {
	strncpy (dest, src, len);
	dest[len] = '\0';
    }

    /* otherwise centre the source string in the space */
    else {
	padlen = (len - strlen (src)) / 2;
	sprintf (dest, "%*s%-*s%*s", padlen, "", len - 2 * padlen, src,
		 padlen + (srclen % 1), "");
    }
}

/**
 * Set the four-line screen title.
 * This occupies the menu window when the menu is not present.
 * @param line1 The first line.
 * @param line2 The second line.
 * @param line3 The third line.
 * @param line4 The fourth line.
 */
static void setscreentitle (char *line1, char *line2, char *line3,
			    char *line4)
{
    centreline (scrtitle[0], line1, 13);
    centreline (scrtitle[1], line2, 13);
    centreline (scrtitle[2], line3, 13);
    centreline (scrtitle[3], line4, 13);
}

/**
 * Show the screen title.
 */
static void showscreentitle (void)
{
    int l; /* line counter */
    bit_ink (scrbuf, 3);
    bit_font (scrbuf, font);
    for (l = 0; l < 4; ++l)
	bit_print (scrbuf, 4, 164 + 8 * l, scrtitle[l]);
    queueupdate (4, 164, 52, 32);
}

/**
 * Display the menu.
 * @param count The number of options in the menu.
 * @param options The menu options.
 * @param option The selected option.
 * @param top The topmost option.
 */
static void displaymenu (int count, char **options, int option, int top)
{
    int c; /* row counter */
    char optstring[14]; /* formatted option string */
    for (c = 0; c < 4; ++c)
	if (top + c < count) {
	    sprintf (optstring, "%-13.13s", options[top + c]);
	    if (top + c == option)
		bit_font (scrbuf, highfont);
	    else
		bit_font (scrbuf, font);
	    bit_ink (scrbuf, 3);
	    bit_print (scrbuf, 4, 164 + 8 * c, optstring);
	} else {
	    bit_ink (scrbuf, 0);
	    bit_box (scrbuf, 4, 164 + 8 * c, 48, 8);
	}
    scr_putpart (screen, scrbuf, 4, 164, 4, 164, 52, 32, DRAW_PSET);
}

/**
 * Show the dialogue box menu and get an option.
 * @param  count   The number of options in the menu.
 * @param  options The menu options.
 * @return         The selected option.
 */
static int getdialoguemenu (int count, char **options)
{
    char optionstring[37], /* the options as a string */
	buf[37]; /* the line buffer */
    int c, /* general counter */
	*x, /* x positions of options */
	option = 0; /* option chosen */

    /* prepare the option string */
    strcpy (optionstring, options[0]);
    for (c = 1; c < count; ++c) {
	strcat (optionstring, "  ");
	strcat (optionstring, options[c]);
    }
    centreline (buf, optionstring, 36);

    /* work out position of menu options */
    x = malloc (count * sizeof (int));
    x[0] = 118 + 4 * ((36 - strlen (optionstring)) / 2);
    for (c = 0; c < count; ++c) {
	if (c)
	    x[c] = x[c - 1] + 4 * (strlen (options[c - 1]) + 2);
    }

    /* show the initial menu */
    scr_ink (screen, 3);
    scr_font (screen, lowfont);
    scr_print (screen, 118, 100, buf);

    /* get an option */
    while (controls->fire ());
    do {

	/* highlight an option */
	scr_font (screen, highfont);
	scr_print (screen, x[option], 100, options[option]);

	/* up/left key pressed */
	if ((controls->left () || controls->up ()) && option > 0) {
	    scr_font (screen, lowfont);
	    scr_print (screen, x[option], 100, options[option]);
	    --option;
	    scr_font (screen, highfont);
	    scr_print (screen, x[option], 100, options[option]);
	    controls->release (0);
	}

	/* right/down key pressed */
	else if ((controls->right () || controls->down()) && option < count - 1) {
	    scr_font (screen, lowfont);
	    scr_print (screen, x[option], 100, options[option]);
	    ++option;
	    scr_font (screen, highfont);
	    scr_print (screen, x[option], 100, options[option]);
	    controls->release (0);
	}

    } while (! controls->fire ());

    /* clean up and return the option */
    free (x);
    return option;
}

/**
 * Show the dialogue box menu and get an option.
 * @param  count   The number of options in the menu.
 * @param  options The menu options.
 * @param  noiseid The noise to play.
 * @return         The selected option.
 */
static int getdialoguemenuwithnoise (int count, char **options, int noiseid)
{
    char optionstring[37], /* the options as a string */
	buf[37]; /* the line buffer */
    int c, /* general counter */
	*x, /* x positions of options */
	option = 0; /* option chosen */

    /* prepare the option string */
    strcpy (optionstring, options[0]);
    for (c = 1; c < count; ++c) {
	strcat (optionstring, "  ");
	strcat (optionstring, options[c]);
    }
    centreline (buf, optionstring, 36);

    /* work out position of menu options */
    x = malloc (count * sizeof (int));
    x[0] = 118 + 4 * ((36 - strlen (optionstring)) / 2);
    for (c = 0; c < count; ++c) {
	if (c)
	    x[c] = x[c - 1] + 4 * (strlen (options[c - 1]) + 2);
    }

    /* show the initial menu */
    scr_ink (screen, 3);
    scr_font (screen, lowfont);
    scr_print (screen, 118, 100, buf);
    display->playsound (noiseid);

    /* get an option */
    while (controls->fire ());
    do {

	/* highlight an option */
	scr_font (screen, highfont);
	scr_print (screen, x[option], 100, options[option]);

	/* up/left key pressed */
	if ((controls->left () || controls->up ()) && option > 0) {
	    scr_font (screen, lowfont);
	    scr_print (screen, x[option], 100, options[option]);
	    --option;
	    scr_font (screen, highfont);
	    scr_print (screen, x[option], 100, options[option]);
	    controls->release (0);
	}

	/* right/down key pressed */
	else if ((controls->right () || controls->down()) && option < count - 1) {
	    scr_font (screen, lowfont);
	    scr_print (screen, x[option], 100, options[option]);
	    ++option;
	    scr_font (screen, highfont);
	    scr_print (screen, x[option], 100, options[option]);
	    controls->release (0);
	}

    } while (! controls->fire ());

    /* clean up and return the option */
    free (x);
    return option;
}

/**
 * Show a single line of the score table.
 * @param line      The line of the score table.
 * @param score     A score line.
 * @param levels    The number of levels to show.
 * @param highlight 1 if line should be highlighted.
 */
static void showscoreline (int line, Score *score, int levels,
			   int highlight)
{
    int total, /* total score */
	c; /* score level counter */
    char linebuf[64], /* buffer for output line */
	scorebuf[8]; /* buffer for level or total score */

    /* calculate total score */
    total = score->total (score, levels);

    /* initialise line buffer */
    if (*score->player)
	sprintf (linebuf, "%4s%-12.12s ", "", score->player);
    else
	sprintf (linebuf, "%4s%-12.12s ", "", "-");

    /* add the level scores */
    for (c = 0; c < 12; ++c) {
	if (c >= levels)
	    strcpy (scorebuf, "   ");
	else if (! *score->player)
	    strcpy (scorebuf, "  -");
	else if (score->scores[c] > 99)
	    strcpy (scorebuf, " **");
	else
	    sprintf (scorebuf, " %2d", score->scores[c]);
	strcat (linebuf, scorebuf);
    }

    /* add the total score */
    if (total)
	sprintf (scorebuf, "  %5d", total);
    else
	strcpy (scorebuf, "      -");
    strcat (linebuf, scorebuf);

    /* output the score line */
    bit_ink (scrbuf, 3);
    if (highlight)
	bit_font (scrbuf, font);
    else
	bit_font (scrbuf, lowfont);
    bit_print (scrbuf, 60, 76 + 8 * line, linebuf);
}

/**
 * Show the cursor anywhere on the screen.
 * @param x The x coordinate of the cursor.
 * @param y The y coordinate of the cursor.
 */
static void showcursor (int x, int y)
{
    scr_put (screen, cursormask, x, y, DRAW_AND);
    scr_put (screen, cursor, x, y, DRAW_OR);
}

/**
 * Hide the cursor by redisplaying the screen buffer there.
 * @param x The x coordinate of the cursor.
 * @param y The y coordinate of the cursor.
 */
static void hidecursor (int x, int y)
{
    queueupdate (x, y, 16, 16);
    display->update ();
}

/*----------------------------------------------------------------------
 * Public Method Function Definitions.
 */

/**
 * Destroy the display when no longer needed.
 */
static void destroy ()
{

    int c; /* generic counter variable */

    /* clean up the display */
    if (display) {

	/* back to text mode */
	scr_destroy (screen);

	/* destroy graphical assets */
	if (title)
	    bit_destroy (title);
	if (panels[PANEL_BLANK])
	    bit_destroy (panels[PANEL_BLANK]);
	if (panels[PANEL_DEPLOY])
	    bit_destroy (panels[PANEL_DEPLOY]);
	if (panels[PANEL_PROGRAM])
	    bit_destroy (panels[PANEL_PROGRAM]);
	if (cursor)
	    bit_destroy (cursor);
	if (cursormask)
	    bit_destroy (cursormask);
	if (bump)
	    bit_destroy (bump);
	if (bumpmask)
	    bit_destroy (bumpmask);
	if (blast)
	    bit_destroy (cursor);
	if (blastmask)
	    bit_destroy (cursormask);
	if (border)
	    bit_destroy (border);
	for (c = 0; c < 12; ++c)
	    if (maptiles[c])
		bit_destroy (maptiles[c]);
	for (c = 0; c < 12; ++c)
	    if (actiontiles[c])
		bit_destroy (actiontiles[c]);
	for (c = 0; c < 6; ++c)
	    if (items[c])
		bit_destroy (items[c]);
	for (c = 0; c < 6; ++c)
	    if (items[c])
		bit_destroy (itemmasks[c]);
	for (c = 0; c < 5; ++c)
	    if (ramtiles[c])
		bit_destroy (ramtiles[c]);
/*
	for (c = 0; c < 4; ++c)
	    if (progressbar[c])
		bit_destroy (progressbar[c]);
*/
	if (dialoguebox)
	    bit_destroy (dialoguebox);

	/* destroy fonts */
	if (font)
	    fnt_destroy (font);
	if (highfont)
	    fnt_destroy (highfont);
	if (lowfont)
	    fnt_destroy (lowfont);

	/* destroy music and sounds */
	if (tune)
	    tune->destroy (tune);
	for (c = 0; c < 9; ++c)
	    if (noises[c])
		noises[c]->destroy (noises[c]);

	/* destroy the display itself */
    	free (display);
    }
}

/**
 * Give display routines access to the game data.
 * @param ingame The game being played.
 */
static void setgame (Game *ingame)
{
    game = ingame;
}

/**
 * Update the screen from the buffer.
 */
static void update (void)
{
    DisplayList *curr, /* current display list entry */
	*next; /* next display list entry */

    /* loop through all display list entries */
    curr = displaylist;
    while (curr) {

	/* update the screen */
	scr_putpart (screen, scrbuf, curr->x, curr->y, curr->x, curr->y,
		     curr->w, curr->h, DRAW_PSET);

	/* look at the next entry */
	next = curr->next;
	free (curr);
	curr = next;
    }

    /* clear the display list */
    displaylist = NULL;
}

/**
 * Display a menu and get an option from it.
 * @param count   The number of options in the menu.
 * @param options The option names in an array.
 * @param initial The initial option selected.
 * @return        The number of the option selected.
 */
static int menu (int count, char **options, int initial)
{
    int top, /* index of top row option */
	option, /* option currently selected */
	pressed; /* 1 if fire is pressed initially */

    /* initialise the menu */
    pressed = controls->fire ();
    top = initial > 3 ? initial - 3 : 0;
    option = initial;
    displaymenu (count, options, option, top);

    /* allow option selection till fire pressed/released */
    while (controls->fire () == pressed) {

	/* up key pressed */
	if ((controls->up () || controls->left ()) && option > 0) {
	    --option;
	    if (option < top)
		top = option;
	    displaymenu (count, options, option, top);
	    while (controls->up () || controls->left ());
	}

	/* down key pressed */
	else if ((controls->down () || controls->right ())
		 && option < count - 1) {
	    ++option;
	    if (option > top + 3)
		top = option - 3;
	    displaymenu (count, options, option, top);
	    while (controls->down () || controls->right ());
	}
    }

    /* clean up and return */
    showscreentitle ();
    update ();
    while (controls->fire ());
    return option;
}

/**
 * Show a dialogue box.
 * @param  message The message to display.
 * @param  count   The number of options.
 * @param  options The menu of buttons.
 * @return         The number of the option selected.
 */
static int dialogue (char *message, int count, char **options)
{
    Bitmap *customdialogue; /* bitmap for the dialogue */
    char buf[37]; /* buffer for the message */
    int option; /* option chosen */

    /* initialise the dialogue */
    if (! (customdialogue = bit_copy (dialoguebox)))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
    centreline (buf, message, 36);
    bit_font (customdialogue, font);
    bit_print (customdialogue, 8, 8, buf);

    /* show the dialogue and get the option */
    scr_put (screen, customdialogue, 110, 84, DRAW_PSET);
    option = getdialoguemenu (count, options);

    /* clean up and return */
    bit_destroy (customdialogue);
    queueupdate (110, 84, 160, 32);
    display->update ();
    return option;
}

/**
 * Show a dialogue box.
 * @param  message The message to display.
 * @param  count   The number of options.
 * @param  options The menu of buttons.
 * @param  noiseid The noise to play.
 * @return         The number of the option selected.
 */
static int dialoguewithnoise (char *message, int count, char **options,
			      int noiseid)
{
    Bitmap *customdialogue; /* bitmap for the dialogue */
    char buf[37]; /* buffer for the message */
    int option; /* option chosen */

    /* initialise the dialogue */
    if (! (customdialogue = bit_copy (dialoguebox)))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
    centreline (buf, message, 36);
    bit_font (customdialogue, font);
    bit_print (customdialogue, 8, 8, buf);

    /* show the dialogue and get the option */
    scr_put (screen, customdialogue, 110, 84, DRAW_PSET);
    option = getdialoguemenuwithnoise (count, options, noiseid);

    /* clean up and return */
    bit_destroy (customdialogue);
    queueupdate (110, 84, 160, 32);
    display->update ();
    return option;
}

/**
 * Show a single square on the level map.
 * @param level    The level to show.
 * @param location The location to show.
 */
static void showlevelmapsquare (Level *level, int location)
{
    int x, /* x coordinate of cell */
	y, /* y coordinate of cell */
	type, /* type of cell, robot or item */
	facing; /* facing of a robot at the cell */
    Robot *robot; /* robot at the cell */
    Item *item; /* item at the cell */

    /* work out cell coordinates on the screen */
    x = 60 + 16 * (location % 16);
    y = 4 + 16 * (location / 16);

    /* show the cell type */
    type = level->cells[location]->type;
    bit_put (scrbuf, maptiles[type - 1], x, y, DRAW_PSET);

    /* show any robot present */
    if ((robot = level->robots[location])) {
	type = robot->type;
	facing = robot->facing;
	bit_put (scrbuf, robotmasks[type - 1][facing], x, y, DRAW_AND);
	bit_put (scrbuf, robots[type - 1][facing], x, y, DRAW_OR);
    }

    /* show any item present */
    else if ((item = level->items[location])) {
	bit_put (scrbuf, itemmasks[item->type - 1], x, y, DRAW_AND);
	bit_put (scrbuf, items[item->type - 1], x, y, DRAW_OR);
    }

    /* queue an update */
    if (! wholemap)
	queueupdate (x, y, 16, 16);
}

/**
 * Show the present state of a level.
 * @param level The level to show.
 */
static void showlevelmap (Level *level)
{
    int c; /* general counter */
    wholemap = 1;
    for (c = 0; c < 192; ++c)
	showlevelmapsquare (level, c);
    wholemap = 0;
    queueupdate (60, 4, 256, 192);
}

/**
 * Make a sound.
 * @param id The ID of the sound.
 */
static void playsound (int id)
{
    if (soundenabled && noises[id])
	noises[id]->play (noises[id]);
}

/**
 * Show the title screen.
 * @param display is the display to affect.
 */
static void showtitlescreen (void)
{
    scr_font (screen, font);
    scr_ink (screen, 3);
    scr_put (screen, title, 0, 0, DRAW_PSET);
    scr_print (screen, 128, 188, " Please wait... ");
}

/**
 * Await the fire key at the title screen.
 * @param display is the display to affect.
 */
static void titlekey (void)
{
    int key; /* value of key pressed */

    /* show the"Press FIRE" message */
    scr_font (screen, font);
    scr_ink (screen, 3);
    scr_print (screen, 128, 188, "   Press FIRE   ");

    /* play the tune or wait for a key until FIRE is pressed */
    do {
	if (soundenabled && tune) {
	    tune->play (tune, controls->getkeyhandler ());
	    while (! controls->getkeyhandler ()->anykey ());
	} else
	    while (! controls->getkeyhandler ()->anykey ());
	key = controls->getkeyhandler ()->scancode ();
    } while (key != KEY_CTRL && key != KEY_ENTER && key != KEY_SPACE);

    /* erase the "Press FIRE" message */
    scr_ink (screen, 0);
    scr_box (screen, 128, 188, 64, 8);
    while (controls->fire ());
}

/**
 * Show the New Game screen.
 * @param name The display name of the game.
 * @param player The name of the player.
 * @param packname The name of the level pack.
 */
static void shownewgame (char *name, char *player, char *packname)
{
    int l; /* display line counter */

    /* prepare the background */
    bit_put (scrbuf, panels[PANEL_BLANK], 0, 0, DRAW_PSET);
    bit_put (scrbuf, border, 60, 0, DRAW_PSET);

    /* show the window title */
    bit_font (scrbuf, lowfont);
    bit_print (scrbuf, 164, 44, "GAME DETAILS");
    setscreentitle ("", "SET UP", "GAME", "");
    showscreentitle ();

    /* show the current options */
    for (l = 0; l < 3; ++l)
    	display->showgameoption (name, player, packname, l, 0);

    /* queue a full screen update */
    queueupdate (0, 0, 320, 200);
}

/**
 * Show a single line of the new game screen.
 * @param name      The display name of the game.
 * @param player    The name of the player.
 * @param packname  The name of the level pack.
 * @param line      The line to show.
 * @param highlight 1 if line should be highlighted.
 */
static void showgameoption (char *game, char *player, char *packname,
			    int line, int highlight)
{
    char text[65]; /* formatted text line */

    /* choose the correct font */
    if (highlight > 0)
	bit_font (scrbuf, highfont);
    else if (highlight < 0)
	bit_font (scrbuf, lowfont);
    else
	bit_font (scrbuf, font);

    /* format the line text */
    switch (line) {
    case 0: /* game name */
	sprintf (text, " Game: %-19.19s ",game);
	break;
    case 1: /* player name */
	sprintf (text, " Player: %-13.13s%5s", player, "");
	break;
    case 2: /* pack name */
	sprintf (text, " Levels: %-13.13s%5s", packname, "");
	break;
    }

    /* output the line text */
    bit_ink (scrbuf, 3);
    bit_print (scrbuf, 132, 84 + 32 * line, text);
    queueupdate (132, 84 + 32 * line, 116, 8);
}

/**
 * Allow editing of the player name.
 * @param name The name to edit.
 */
static void renameplayer (char *name)
{
    edittext (name, 13, 168, 116);
    bit_font (scrbuf, font);
    bit_print (scrbuf, 168, 116, name);
    queueupdate (168, 116, 52, 8);
}

/**
 * Show the Score screen.
 * @param title1    The first half of the screen title.
 * @param title2    The second half of the screen title.
 * @param packname  The name of the level pack.
 * @param scores    The score table to show.
 * @param levels    The number of levels to count.
 * @param highlight The line to highlight.
 */
static void showscorescreen (char *title1, char *title2, char *packname,
			     ScoreTable *scores, int levels,
			     int highlight)
{
    int l; /* display line counter */
    char linebuffer[64];

    /* prepare the background */
    bit_put (scrbuf, panels[PANEL_BLANK], 0, 0, DRAW_PSET);
    bit_put (scrbuf, border, 60, 0, DRAW_PSET);

    /* show the window title */
    bit_font (scrbuf, lowfont);
    setscreentitle ("", title1, title2, "");
    showscreentitle ();

    /* table heading */
    bit_font (scrbuf, font);
    centreline (linebuffer, packname, 63);
    bit_print (scrbuf, 60, 28, linebuffer);
    centreline (linebuffer, "Twelve Best Scores", 63);
    bit_print (scrbuf, 60, 36, linebuffer);
    bit_font (scrbuf, lowfont);
    sprintf (linebuffer, "%48sLevel   Pack", "");
    bit_print (scrbuf, 60, 52, linebuffer);
    sprintf (linebuffer,
	     "%4sPlayer%8s01 02 03 04 05 06 07 08 09 10 11 12  Total",
	     "", "");
    bit_print (scrbuf, 60, 60, linebuffer);

    /* table entries */    
    for (l = 0; l < 12; ++l)
	showscoreline (l, scores->scores[l], levels, highlight == l);

    /* update the display */
    queueupdate (0, 0, 320, 200);
}

/**
 * Show the Deployment Screen.
 * @param robots The robots still on the deployment panel.
 * @param level  The level to deploy robots on.
 */
static void showdeploymentscreen (Robot **robots, Level *level)
{
    int c; /* general counter */
    char buf[14]; /* text buffer */

    /* show the deployment panel and the screen border */
    bit_put (scrbuf, panels[PANEL_DEPLOY], 0, 0, DRAW_PSET);
    bit_put (scrbuf, border, 60, 0, DRAW_PSET);

    /* show the window title */
    bit_font (scrbuf, lowfont);
    setscreentitle ("", "ROBOT", "DEPLOYMENT", "");
    showscreentitle ();

    /* show level details */
    bit_font (scrbuf, font);
    bit_print (scrbuf, 8, 8, game->levelpack->name);
    sprintf (buf, "Level %02d", game->levelid + 1);
    bit_print (scrbuf, 8, 16, buf);

    /* show the robots on the panel */
    for (c = 0; c < 6; ++c)
	if (robots[c])
	    display->showdeploymentrobot(c + 1, 1);

    /* show the level */
    showlevelmap (level);

    /* queue an update and return */
    queueupdate (0, 0, 320, 200);
}

/**
 * Show a robot (or an empty space) on the deployment panel.
 * @params type    The type of robot 1..6.
 * @params present 1 if the robot is on the panel, 0 if not.
 */
static void showdeploymentrobot(int type, int present)
{
    int x, /* x coordinate of robot on panel */
	y; /* y coordinate of robot on panel */

    /* work out the position of the robot */
    x = 8 + 28 * ((type - 1) / 3);
    y = 80 + 28 * ((type - 1) % 3);

    /* show the robot or a blank spot */
    if (present)
	bit_put (scrbuf, robots[type - 1][2], x, y, DRAW_PSET);
    else {
	bit_ink (scrbuf, 0);
	bit_box (scrbuf, x, y, 16, 16);
    }

    /* update the display queue */
    queueupdate (x, y, 16, 16);
}

/**
 * Show the cursor on the deployment screen.
 * @param pos   The cursor position: <0 = robot panel, >=0 = map.
 * @param shown 1 if shown, 0 if hidden.
 */
static void deploymentcursor (int pos, int shown)
{
    int x, /* x position of cursor */
	y; /* y position of cursor */

    /* work out where the cursor is */
    if (pos < 0) {
	x = 8 + 28 * ((-pos - 1) / 3);
	y = 80 + 28 * ((-pos -1) % 3);
    } else {
	x = 60 + 16 * (pos % 16);
	y = 4 + 16 * (pos / 16);
    }

    /* show or hide the cursor */
    if (shown)
	showcursor (x, y);
    else
	hidecursor (x, y);
}

/**
 * Navigate the cursor around the deployment panel.
 * @param  cursor The initial cursor location.
 * @return        The final cursor location.
 */
static int navigatedeploypanel (int cursor)
{
    deploymentcursor (cursor, 1);
    do {
	if (controls->left () && cursor < -3) {
	    deploymentcursor (cursor, 0);
	    cursor += 3;
	    deploymentcursor (cursor, 1);
	    controls->release (250);
	} else if (controls->right () && cursor > -4) {
	    deploymentcursor (cursor, 0);
	    cursor -= 3;
	    deploymentcursor (cursor, 1);
	    controls->release (250);
	} else if (controls->right ()) {
	    deploymentcursor (cursor, 0);
	    if (cursor == -4) cursor = 80;
	    if (cursor == -5) cursor = 96;
	    if (cursor == -6) cursor = 128;
	    deploymentcursor (cursor, 1);
	    controls->release (250);
	} else if (controls->up () && cursor != -1 && cursor != -4) {
	    deploymentcursor (cursor, 0);
	    cursor += 1;
	    deploymentcursor (cursor, 1);
	    controls->release (250);
	} else if (controls->down () && cursor != -3 && cursor != -6) {
	    deploymentcursor (cursor, 0);
	    cursor -= 1;
	    deploymentcursor (cursor, 1);
	    controls->release (250);
	}
    } while (cursor < 0 && ! controls->fire ());
    deploymentcursor (cursor, 0);
    return cursor;
}

/**
 * Navigate the cursor around the deployment panel.
 * @param  cursor The initial cursor location.
 * @return        The final cursor location.
 */
static int navigatedeploymap (int cursor)
{
    deploymentcursor (cursor, 1);
    do {
	if (controls->left () && cursor % 16 > 0) {
	    deploymentcursor (cursor, 0);
	    cursor -= 1;
	    deploymentcursor (cursor, 1);
	    controls->release (250);
	} else if (controls->left () && cursor % 16 == 0) {
	    deploymentcursor (cursor, 0);
	    cursor = -4 - (cursor / 16 >= 6) - (cursor / 16 >= 8);
	    deploymentcursor (cursor, 1);
	    controls->release (250);
	} else if (controls->right () && cursor % 16 < 15) {
	    deploymentcursor (cursor, 0);
	    cursor += 1;
	    deploymentcursor (cursor, 1);
	    controls->release (250);
	} else if (controls->up () && cursor / 16 > 0) {
	    deploymentcursor (cursor, 0);
	    cursor -= 16;
	    deploymentcursor (cursor, 1);
	    controls->release (250);
	} else if (controls->down () && cursor / 16 < 11) {
	    deploymentcursor (cursor, 0);
	    cursor += 16;
	    deploymentcursor (cursor, 1);
	    controls->release (250);
	}
    } while (cursor >= 0 && ! controls->fire ());
    deploymentcursor (cursor, 0);
    return cursor;
}

/**
 * Show the current droid in the deployment screen.
 * @param robot The current droid.
 */
static void showrobottodeploy (Robot *robot)
{
    char buf[10]; /* string buffer */
    bit_put (scrbuf, robots[robot->type - 1][2], 4, 48, DRAW_PSET);
    sprintf (buf, "%-9.9s", robot->name);
    bit_font (scrbuf, font);
    bit_ink (scrbuf, 3);
    bit_print (scrbuf, 20, 48, buf);
    sprintf (buf, "%-9.9s", robot->x == 0xff ? "Reserved" : "Deployed");
    bit_font (scrbuf, lowfont);
    bit_print (scrbuf, 20, 56, buf);
    queueupdate (4, 48, 52, 16);
}

/**
 * Show the programming screen.
 * @param  library The library of actions available this turn.
 * @param  robot   The current robot.
 * @param  level   The level layout.
 */
static void showprogrammingscreen (Action **library, Robot *robot,
				   Level *level)
{
    char buf[14]; /* text buffer */

    /* show the deployment panel and the screen border */
    bit_put (scrbuf, panels[PANEL_PROGRAM], 0, 0, DRAW_PSET);
    bit_put (scrbuf, border, 60, 0, DRAW_PSET);

    /* show the window title */
    bit_font (scrbuf, lowfont);
    setscreentitle ("", "ROBOT", "PROGRAMMING", "");
    showscreentitle ();

    /* show level details */
    bit_font (scrbuf, font);
    bit_print (scrbuf, 4, 4, game->levelpack->name);
    sprintf (buf, "Level %02d", game->levelid + 1);
    bit_print (scrbuf, 4, 12, buf);

    /* show the level */
    showlevelmap (level);

    /* show the library and the current robot */
    display->showlibrary (library);
    if (robot)
	display->showrobottoprogram (robot, level);

    /* queue an update and return */
    queueupdate (0, 0, 320, 200);
}

/**
 * Show the library on the programming screen.
 * @param  library The library of actions available.
 */
static void showlibrary (Action **library)
{
    int c; /* general counter */
    for (c = 0; c < 6; ++c)
	bit_put (scrbuf, actiontiles[library[c]->type - 1],
		 40, 112 - 16 * c, DRAW_PSET);
    queueupdate (40, 32, 16, 96);
}

/**
 * Show the cursor on the program screen.
 * @param pos   The cursor position:
 *              -8..-15 in the RAM.
 *              -2..-7  in the library.
 *              -1      in the ROM.
 *              0..191  on the map.
 * @param shown 1 if shown, 0 if hidden.
 */
static void programcursor (int pos, int shown)
{
    int x, /* x position of cursor */
	y; /* y position of cursor */

    /* work out where the cursor is */
    if (pos < -7) {
	x = 12;
	y = 32 + 16 * (15 + pos);
    } else if (pos < -1) {
	x = 40;
	y = 32 + 16 * (7 + pos);
    } else if (pos == -1) {
	x = 40;
	y = 144;
    } else {
	x = 60 + 16 * (pos % 16);
	y = 4 + 16 * (pos / 16);
    }

    /* show or hide the cursor */
    if (shown)
	showcursor (x, y);
    else
	hidecursor (x, y);
}

/**
 * Navigate around the robot's RAM.
 * @param  cursor  The initial cursor location
 * @param  ramsize The size of the robot's RAM.
 * @return         The final cursor location.
 */
static int navigateram (int cursor, int ramsize)
{
    programcursor (cursor, 1);
    do {
	if (controls->right ()) {
	    programcursor (cursor, 0);
	    cursor += 8 - (cursor == -8);
	    programcursor (cursor, 1);
	    controls->release (250);
	} else if (controls->up () && cursor > -15) {
	    programcursor (cursor, 0);
	    --cursor;
	    programcursor (cursor, 1);
	    controls->release (250);
	} else if (controls->down () && cursor < -8) {
	    programcursor (cursor, 0);
	    ++cursor;
	    programcursor (cursor, 1);
	    controls->release (250);
	}
    } while (cursor <= -8 && ! controls->fire ());
    programcursor (cursor, 0);
    return cursor;
}

/**
 * Navigate around the library.
 * @param cursor The initial cursor location.
 * @return       The final cursor location.
 */
static int navigatelibrary (int cursor)
{
    programcursor (cursor, 1);
    do {
	if (controls->left ()) {
	    programcursor (cursor, 0);
	    cursor -= 8;
	    programcursor (cursor, 1);
	    controls->release (250);
	} else if (controls->right ()) {
	    programcursor (cursor, 0);
	    cursor += 9;
	    cursor = 16 * cursor;
	    programcursor (cursor, 1);
	    controls->release (250);
	} else if (controls->up () && cursor > -7) {
	    programcursor (cursor, 0);
	    --cursor;
	    programcursor (cursor, 1);
	    controls->release (250);
	} else if (controls->down ()) {
	    programcursor (cursor, 0);
	    ++cursor;
	    programcursor (cursor, 1);
	    controls->release (250);
	}
    } while (cursor <= -2 && cursor >= -7 && ! controls->fire ());
    programcursor (cursor, 0);
    return cursor;
}

/**
 * Navigate from the ROM.
 * @param cursor The initial cursor location.
 * @return       The final cursor location.
 */
static int navigaterom (int cursor)
{
    programcursor (cursor, 1);
    do {
	if (controls->left ()) {
	    programcursor (cursor, 0);
	    cursor = -8;
	    programcursor (cursor, 1);
	    controls->release (250);
	} else if (controls->right ()) {
	    programcursor (cursor, 0);
	    cursor = 144;
	    programcursor (cursor, 1);
	    controls->release (250);
	} else if (controls->up ()) {
	    programcursor (cursor, 0);
	    --cursor;
	    programcursor (cursor, 1);
	    controls->release (250);
	}
    } while (cursor == -1 && ! controls->fire ());
    programcursor (cursor, 0);
    return cursor;
}

/**
 * Navigate around the programming map.
 * @param cursor The initial cursor location.
 * @return       The final cursor location.
 */
static int navigateprogrammap (int cursor)
{
    programcursor (cursor, 1);
    do {
	if (controls->left () && cursor % 16 > 0) {
	    programcursor (cursor, 0);
	    cursor -= 1;
	    programcursor (cursor, 1);
	    controls->release (250);
	} else if (controls->left () && cursor % 16 == 0) {
	    programcursor (cursor, 0);
	    if (cursor / 16 >= 8)
		cursor = -1; /* rom */
	    else if (cursor / 16 <= 2)
		cursor = -7; /* top of library stack */
	    else
		cursor = (cursor / 16) - 9; /* somewhere in the stack */
	    programcursor (cursor, 1);
	    controls->release (250);
	} else if (controls->right () && cursor % 16 < 15) {
	    programcursor (cursor, 0);
	    cursor += 1;
	    programcursor (cursor, 1);
	    controls->release (250);
	} else if (controls->up () && cursor / 16 > 0) {
	    programcursor (cursor, 0);
	    cursor -= 16;
	    programcursor (cursor, 1);
	    controls->release (250);
	} else if (controls->down () && cursor / 16 < 11) {
	    programcursor (cursor, 0);
	    cursor += 16;
	    programcursor (cursor, 1);
	    controls->release (250);
	}
    } while (cursor >= 0 && ! controls->fire ());
    programcursor (cursor, 0);
    return cursor;
}

/**
 * Show the robot that's about to be programmed.
 * @param robot The robot selected.
 * @param level The level being played (for possible inventory)
 */
static void showrobottoprogram (Robot *robot, Level *level)
{
    char buf[13]; /* buffer for robot name */
    int c, /* general counter */
	y; /* y coordinate of RAM tile */
    Item *item; /* the item the robot is holding */

    /* show the robot name and icon */
    sprintf (buf, "%-9.9s", robot->name);
    bit_font (scrbuf, font);
    bit_ink (scrbuf, 3);
    bit_print (scrbuf, 4, 20, buf);
    bit_put (scrbuf, robots[robot->type - 1][2], 40, 12, DRAW_PSET);

    /* show the RAM */
    for (c = 0; c < 8; ++c) {
	y = 144 - 16 * c;
	if (c >= robot->ramsize)
	    bit_put (scrbuf, ramtiles[0], 12, y, DRAW_PSET);
	else if (robot->ram[c])
	    bit_put (scrbuf, actiontiles[robot->ram[c] - 1], 12, y,
		     DRAW_PSET);
	else if (c == robot->ramsize - 1)
	    bit_put (scrbuf, ramtiles[1], 12, y, DRAW_PSET);
	else if (c == 0 && robot->ramsize == 1)
	    bit_put (scrbuf, ramtiles[4], 12, y, DRAW_PSET);
	else if (c == 0)
	    bit_put (scrbuf, ramtiles[3], 12, y, DRAW_PSET);
	else
	    bit_put (scrbuf, ramtiles[2], 12, y, DRAW_PSET);
    }

    /* show the inventory */
    if ((item = level->items[robot->x + 16 * robot->y]))
	bit_put (scrbuf, items[item->type - 1], 40, 128, DRAW_PSET);
    else if (robot->hasinventory)
	bit_put (scrbuf, ramtiles[4], 40, 128, DRAW_PSET);
    else
	bit_put (scrbuf, ramtiles[0], 40, 128, DRAW_PSET);

    /* show the ROM */
    if (robot->rom)
	bit_put (scrbuf, actiontiles[robot->rom - 1], 40, 144,
		 DRAW_PSET);
    else
	bit_put (scrbuf, ramtiles[0], 40, 144, DRAW_PSET);

    /* queue update to the panel */
    queueupdate (4, 12, 52, 148);
}

/**
 * Show the action screen.
 * @param  level   The level layout.
 */
static void showactionscreen (Level *level)
{
    /* show the deployment panel and the screen border */
    bit_put (scrbuf, panels[PANEL_BLANK], 0, 0, DRAW_PSET);
    bit_put (scrbuf, border, 60, 0, DRAW_PSET);

    /* show the window title */
    bit_font (scrbuf, lowfont);
    setscreentitle ("", "ACTION!", "", "");
    showscreentitle ();
    display->showprogressbar (0);

    /* show the level */
    showlevelmap (level);

    /* queue an update and return */
    queueupdate (0, 0, 320, 200);
}

/**
 * Show a phaser beam in transit.
 * @param x The x position of the beam.
 * @param y The y position of the beam.
 * @param facing 0 for north/south, 1 for east/west.
 */
static void showphaserbeam (int x, int y, int facing)
{
    scr_put (screen, phasermasks[facing], 60 + 16 * x, 4 + 16 * y,
	     DRAW_AND);
    scr_put (screen, phaserbeams[facing], 60 + 16 * x, 4 + 16 * y,
	     DRAW_OR);
}

/**
 * Hide a phaser beam by redisplaying the level map square
 * @param level    The level to show.
 * @param location The location to show.
 */
static void hidephaserbeam (Level *level, int location)
{
    int x, /* x coordinate of cell */
	y, /* y coordinate of cell */
	type; /* cell type */

    /* work out cell coordinates on the screen */
    x = 60 + 16 * (location % 16);
    y = 4 + 16 * (location / 16);

    /* show the cell type */
    type = level->cells[location]->type;
    scr_put (screen, maptiles[type - 1], x, y, DRAW_PSET);

}

/**
 * Show a blast as a phaser beam hits something.
 * @param x The x position of the blast.
 * @param y The y position of the blast.
 */
static void showblast (int x, int y)
{
    scr_put (screen, blastmask, 60 + 16 * x, 4 + 16 * y,
	     DRAW_AND);
    scr_put (screen, blast, 60 + 16 * x, 4 + 16 * y,
	     DRAW_OR);
}

/**
 * Show a progress bar of x/8 steps.
 * @param count The number of steps already taken.
 */
static void showprogressbar (int count)
{
    int c; /* counter */
    bit_put (scrbuf, progressbar[0], 8, 180, DRAW_PSET);
    bit_put (scrbuf, progressbar[3], 44, 180, DRAW_PSET);
    for (c = 0; c < 8; ++c)
	if (c < count)
	    bit_put (scrbuf, progressbar[2], 12 + 4 * c, 180,
		     DRAW_PSET);
	else
	    bit_put (scrbuf, progressbar[1], 12 + 4 * c, 180,
		     DRAW_PSET);
    queueupdate (8, 180, 40, 8);
}

/*----------------------------------------------------------------------
 * Constructor Definitions.
 */

/**
 * Display constructor.
 * @param colourset = 0 for mono, 1 for colour, 2 for nice colour.
 * @param quiet = 0 for sound and music, 1 for silence.
 * @return the new display.
 */
Display *new_Display (int colourset, int quiet)
{
    /* local variables */
    int mode; /* the screen mode */

    /* allocate memory */
    if (display)
	return display;
    if (! (display = malloc (sizeof (Display))))
	return NULL;

    /* initialise methods - general */
    display->destroy = destroy;
    display->setgame = setgame;
    display->update = update;
    display->menu = menu;
    display->dialogue = dialogue;
    display->dialoguewithnoise = dialoguewithnoise;
    display->showlevelmap = showlevelmap;
    display->showlevelmapsquare = showlevelmapsquare;
    display->playsound = playsound;

    /* intialise methods - title screen */
    display->showtitlescreen = showtitlescreen;
    display->titlekey = titlekey;

    /* initialise methods - new game screen */
    display->shownewgame = shownewgame;
    display->showgameoption = showgameoption;
    display->renameplayer = renameplayer;

    /* initialise methods - score screen */
    display->showscorescreen = showscorescreen;

    /* initialise methods - deployment screen */
    display->showdeploymentscreen = showdeploymentscreen;
    display->showdeploymentrobot = showdeploymentrobot;
    display->deploymentcursor = deploymentcursor;
    display->navigatedeploypanel = navigatedeploypanel;
    display->navigatedeploymap = navigatedeploymap;
    display->showrobottodeploy = showrobottodeploy;

    /* initialise methods - programming screen */
    display->showprogrammingscreen = showprogrammingscreen;
    display->showlibrary = showlibrary;
    display->programcursor = programcursor;
    display->navigateram = navigateram;
    display->navigatelibrary = navigatelibrary;
    display->navigaterom = navigaterom;
    display->navigateprogrammap = navigateprogrammap;
    display->showrobottoprogram = showrobottoprogram;

    /* initialise methods - action screen */
    display->showactionscreen = showactionscreen;
    display->showphaserbeam = showphaserbeam;
    display->hidephaserbeam = hidephaserbeam;
    display->showblast = showblast;
    display->showprogressbar = showprogressbar;

    /* initialise the game controls handler */
    controls = getcontrols ();

    /* store sound settings */
    soundenabled = ! quiet;

    /* prepare to initialise the CGALIB screen */
    if (colourset == 0)
	mode = 6;
    else if (colourset == 1)
	mode = 4;
    else if (colourset == 2)
	mode = 5;

    /* initialise the CGALIB screen */
    if (! (screen = scr_create (mode))) {
	free (display);
	return NULL;
    }
    fataldisplay (display);
    if (colourset == 2)
	scr_palette (screen, 5, 3);

    /* create the off-screen buffer */
    scrbuf = bit_create (320, 200);

    /* initialise the assets */
    loadassets ();

    /* initialise the screen title */
    setscreentitle ("", "", "", "");

    /* return the screen */
    return display;
}
