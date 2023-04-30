/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * New Game User Interface Module.
 */

/*----------------------------------------------------------------------
 * Headers
 */

/* ANSI C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <time.h>
#include <io.h>
#include <ctype.h>

/* project specific headers */
#include "uiscreen.h"
#include "tdroid.h"
#include "display.h"
#include "controls.h"
#include "config.h"
#include "game.h"
#include "levelpak.h"
#include "fatal.h"


/*----------------------------------------------------------------------
 * Data Definitions.
 */

/**
 * @struct uiscreendata
 * Private data for this UI Screen.
 */
struct uiscreendata {

    /** @var game The game to set up. */
    Game *game;

    /** @var gameindex The index of the game file in the list. */
    int gameindex;

    /** @var packindex The index of the level pack in the list. */
    int packindex;

    /** @var highlight The highlighted setting. */
    int highlight;

};

/**
 * @struct pack_ref is a reference to an unloaded pack.
 */
typedef struct pack_ref PackRef;
struct pack_ref {

    /** @var filename The pack filename. */
    char filename[13];

    /** @var name The pack display name. */
    char name[33];

};

/**
 * @struct game_ref A reference to an unloaded game.
 */
typedef struct game_ref GameRef;
struct game_ref {

    /** @var filename The game filename */
    char filename[13];

    /** @var name The game display name */
    char name[33];

    /** @var player The player's name */
    char player[14];

    /** @var packindex Which level pack the game plays. */
    int packindex;

};

/** @var display A pointer to the display module. */
static Display *display;

/** @var controls A pointer to the game controls module. */
static Controls *controls;

/** @var config A pointer to the configuration. */
static Config *config;

/** @var gamerefs References to in-progress games found on disk. */
static GameRef gamerefs[16];

/** @var packrefs References to packs found on disk. */
static PackRef packrefs[16];

/** @var newgamemenu The menu for a new game. */
static char *newgamemenu[] = {
    "Cancel menu",
    "Start game",
    "Delete game",
    "Exit game"
};

/*----------------------------------------------------------------------
 * Level 2 Private Function Definitions.
 */

/**
 * Identify a levelpack file in the list by its filename.
 * @param  filename The filename of the levelpack.
 * @return          Its index in the list, or -1 if not found.
 */
static int identifylevelpack (char *filename)
{
    int c = 0; /* levelpack index counter */
    for (c = 0; c < 16; ++c)
	if (! strcmp (packrefs[c].filename, filename))
	    return c;
    return -1;
}

/**
 * Adjust the game.
 * @param uiscreen  A pointer to the screen.
 * @param direction The direction to move along the list.
 */
static void adjustgame (UIScreen *uiscreen, int direction)
{
    int c; /* field counter */
    
    /* adjust the game entry itself */
    uiscreen->data->gameindex += direction;
    if (uiscreen->data->gameindex >= 16 ||
	uiscreen->data->gameindex < 0 ||
	gamerefs[uiscreen->data->gameindex].name[0] == 0) {
	uiscreen->data->gameindex -= direction;
	return;
    }

    /* update the other fields internally */
    uiscreen->data->packindex =
	gamerefs[uiscreen->data->gameindex].packindex;

    /* update the other fields on the screen */
    for (c = 1; c < 3; ++c)
	display->showgameoption
	    (gamerefs[uiscreen->data->gameindex].name,
	     gamerefs[uiscreen->data->gameindex].player,
	     packrefs[uiscreen->data->packindex].name,
	     c,
	     -(uiscreen->data->gameindex != 0));
}

/**
 * Rename the player.
 * @param uiscreen  A pointer to the screen.
 */
static void renameplayer (UIScreen *uiscreen)
{
    display->renameplayer (gamerefs[uiscreen->data->gameindex].player);
}

/**
 * Adjust the level pack.
 * @param direction is the direction on the list.
 */
static void adjustlevelpack (UIScreen *uiscreen, int direction)
{
    /* adjust the pack name itself */
    uiscreen->data->packindex += direction;
    if (uiscreen->data->packindex >= 16 ||
	uiscreen->data->packindex < 0 ||
	packrefs[uiscreen->data->packindex].filename[0] == 0) {
	uiscreen->data->packindex -= direction;
	return;
    }
    gamerefs[uiscreen->data->gameindex].packindex
	= uiscreen->data->packindex;

    /* adjust the saved config setting */
    strcpy (config->levelpackfile,
	    packrefs[uiscreen->data->packindex].filename);
}

/*----------------------------------------------------------------------
 * Level 1 Function Definitions.
 */

/**
 * Initialise the list of levelpacks available.
 */
static void initialiselevelpackfiles (void)
{
    DIR *dir; /* directory handle */
    struct dirent *direntry; /* a directory entry */
    char *ext; /* pointer to filename extension */
    int c = 0; /* levelpack counter */
    LevelPack *levelpack; /* temporary levelpack variable */

    /* Initialise the temporary levelpack variable */
    if (! (levelpack = new_LevelPack ()))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* attempt to open the directory */
    if (! (dir = opendir (".")))
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);

    /* read each entry and see if it is a levelpack */
    while (c < 16 && (direntry = readdir (dir))) {

	/* skip past files that are not *.CAM files */
	if (! (ext = strchr (direntry->d_name, '.')))
	    continue;
	if (strcmp (ext, ".lev") && strcmp (ext, ".LEV"))
	    continue;

	/* attempt to load the levelpack and add it to the list */
	strcpy (levelpack->filename, direntry->d_name);
	if (! levelpack->load (levelpack, 1))
	    continue;
	strcpy (packrefs[c].filename, levelpack->filename);
	strcpy (packrefs[c].name, levelpack->name);
	++c;
    }

    /* close the directory when done */
    closedir (dir);
    levelpack->destroy (levelpack);
}

/**
 * Initialise the list if in-progress games.
 */
static void initialisegamefiles (void)
{
    DIR *dir; /* directory handle */
    struct dirent *direntry; /* a directory entry */
    char *ext, /* pointer to filename extension */
	*ptr; /* pointer that strtol needs */
    Game *game; /* temporary game buffer */
    int g = 0; /* game counter */
    time_t t; /* time in seconds since epoch */

    /* the first entry is always "New Game" */
    *gamerefs[g].filename = '\0';
    strcpy (gamerefs[g].name, "New game");
    strcpy (gamerefs[g].player, config->player);
    gamerefs[g].packindex = 0;
    ++g;

    /* Initialise the temporary game variable */
    if (! (game = new_Game ()))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* attempt to open the directory */
    if (! (dir = opendir (".")))
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);

    /* read each entry and see if it is a levelpack */
    while (g < 16 && (direntry = readdir (dir))) {

	/* skip past files that are not *.CAM files */
	if (! (ext = strchr (direntry->d_name, '.')))
	    continue;
	if (strcmp (ext, ".gam") && strcmp (ext, ".GAM"))
	    continue;

	/* add the game to the list */
	strcpy (game->filename, direntry->d_name);
	if (! game->load (game, 1))
	    continue; /* invalid/not a game file */
	strcpy (gamerefs[g].filename, direntry->d_name);
	t = strtol (direntry->d_name, &ptr, 16);
	strftime (gamerefs[g].name, 32, "%Y-%m-%d %H:%M:%S",
		  localtime(&t));
	strcpy (gamerefs[g].player, game->player);
	gamerefs[g].packindex =
	    identifylevelpack (game->levelpackfile);
	if (gamerefs[g].packindex == -1)
	    continue; /* invalid/deleted levelpack file */
	++g;
    }

    /* close the directory when done */
    game->destroy (game);
    closedir (dir);
}

/**
 * Get the game options until FIRE is pressed.
 */
static void getgameoptions (UIScreen *uiscreen)
{
    int step, /* direction of change: left -1, right +1 */
	key; /* keypress other than cursor controls and fire */

    /* main game option loop */
    do {

	/* highlight option and await control press */
	display->showgameoption
	    (gamerefs[uiscreen->data->gameindex].name,
	     gamerefs[uiscreen->data->gameindex].player,
	     packrefs[uiscreen->data->packindex].name,
	     uiscreen->data->highlight,
	     1);
	display->update ();
	controls->release (0);
	controls->wait ();
	display->showgameoption
	    (gamerefs[uiscreen->data->gameindex].name,
	     gamerefs[uiscreen->data->gameindex].player,
	     packrefs[uiscreen->data->packindex].name,
	     uiscreen->data->highlight,
	     -(uiscreen->data->gameindex != 0));

	/* up/down controls */
	if (controls->up () && uiscreen->data->highlight > 0)
	    --uiscreen->data->highlight;
	else if (controls->down () &&
		 uiscreen->data->highlight < 2 &&
		 uiscreen->data->gameindex == 0)
	    ++uiscreen->data->highlight;

	/* left/right controls */
	else if ((step = controls->right () - controls->left ())) {
	    switch (uiscreen->data->highlight) {
	    case 0: /* game */
		adjustgame (uiscreen, step);
		break;
	    case 1: /* player */
		renameplayer (uiscreen);
		break;
	    case 2: /* level pack */
		adjustlevelpack (uiscreen, step);
		break;
	    }
	}

	/* printable characters */
	else if (uiscreen->data->highlight == 1) {
	    key = controls->key ();
	    if (key > ' ' && key <= '~') {
		sprintf (gamerefs[uiscreen->data->gameindex].player,
			 "%c", toupper (key));
		renameplayer (uiscreen);
	    }
	}

    } while (! controls->fire ());

    /* do a final screen update, as there may be a delay now */
    display->update ();

    /* save "New Game" settings to configuration */
    strcpy (config->player, gamerefs[0].player);
    strcpy (config->levelpackfile,
	    packrefs[gamerefs[0].packindex].filename);
}

/**
 * Load an old game.
 * @param screen The user interface screen and its data.
 */
static UIState loadgame (UIScreen *uiscreen)
{
    Game *game; /* pointer to the game */

    /* load the game */
    game = uiscreen->data->game;
    strcpy (game->filename,
	    gamerefs[uiscreen->data->gameindex].filename);
    game->load (game, 0);
    strcpy (config->gamefile, game->filename);
    display->setgame (game);

    /* return the initial game state */
    return game->state;
}

/**
 * Prepare a new game when options have been chosen.
 * @param uiscreen A pointer to the screen data.
 */
static int preparegame (UIScreen *uiscreen)
{
    /* local variables */
    Game *game; /* pointer to the game */
    LevelPack *levelpack; /* the level pack */

    /* initialise the basic game data */
    game = uiscreen->data->game;
    game->clear (game);
    sprintf (game->filename, "%08lx.gam", time (NULL));
    strcpy (game->player, gamerefs[uiscreen->data->gameindex].player);
    strcpy (game->score->player, game->player);
    strcpy (game->levelpackfile,
	    packrefs[uiscreen->data->packindex].filename);
    game->state = STATE_SCORE;
    game->levelid = 0;
    game->turnno = 0;

    /* initialise the levelpack and the first level */
    if (game->levelpack)
	game->levelpack->clear (game->levelpack);
    else if (! (game->levelpack = new_LevelPack ()))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
    levelpack = game->levelpack;
    strcpy (levelpack->filename, game->levelpackfile);
    if (! levelpack->load (levelpack, 0))
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    if (game->level)
	game->level->destroy (game->level);
    game->level =
	levelpack->levels[game->levelid]
	->clone (levelpack->levels[game->levelid]);

    /* save the game information in the configuration */
    strcpy (config->levelpackfile, game->levelpackfile);
    strcpy (config->gamefile, game->filename);

    /* give the display access to the game data */
    display->setgame (game);

    /* return the initial game state */
    return game->state;
}

/**
 * Delete an old game.
 * @param screen The user interface screen and its data.
 */
static void deletegame (UIScreen *uiscreen)
{
    int c; /* display line counter */

    /* remove the game file */
    unlink (gamerefs[uiscreen->data->gameindex].filename);

    /* remove the game from the game refs */
    for (c = uiscreen->data->gameindex; c < 15; ++c)
	gamerefs[c] = gamerefs[c + 1];
    *gamerefs[15].filename = '\0';
    *gamerefs[15].name = '\0';
    *gamerefs[15].player = '\0';
    gamerefs[15].packindex = 0;

    /* ensure we're not pointing at an empty game */
    if (! *gamerefs[uiscreen->data->gameindex].filename &&
	uiscreen->data->gameindex > 0)
	--uiscreen->data->gameindex;

    /* update the display */
    for (c = 0; c < 3; ++c)
	display->showgameoption
	    (gamerefs[uiscreen->data->gameindex].name,
	     gamerefs[uiscreen->data->gameindex].player,
	     packrefs[uiscreen->data->packindex].name,
	     c, 0);
    display->update ();
}

/*----------------------------------------------------------------------
 * Public Method Function Definitions.
 */

/**
 * Initialise the UI state when first encountered.
 * @param uiscreen The screen to affect.
 */
static void init (UIScreen *uiscreen)
{
    int c; /* levelpack counter */

    /* save the current game and reset config to 'no game loaded' */
    if (*uiscreen->data->game->filename)
	uiscreen->data->game->save (uiscreen->data->game);
    *config->gamefile = '\0';

    /* initialise the levelpack and game lists */
    initialiselevelpackfiles ();
    initialisegamefiles ();

    /* initialise the list indexes */
    for (c = 0; c < 16; ++c)
	if (! strcmp (config->levelpackfile, packrefs[c].filename))
	    uiscreen->data->packindex
		= gamerefs[0].packindex
		= c;
    uiscreen->data->gameindex = 0;

}

/**
 * Show the UI screen.
 * @param uiscreen The screen to show.
 * @return         The ID of the screen to show next.
 */
static UIState show (UIScreen *uiscreen)
{
    int option; /* option chosen from the menu */

    /* initialise the display */
    display->shownewgame
	(gamerefs[uiscreen->data->gameindex].name,
	 gamerefs[uiscreen->data->gameindex].player,
	 packrefs[uiscreen->data->packindex].name);
    display->update ();

    /* main loop */
    while (1) {

	/* allow navigation of the game options */
	getgameoptions (uiscreen);

	/* get a choice from the menu */
	option = display->menu (4, newgamemenu, 1);
	switch (option) {

	case 0: /* cancel menu */
	    break;

	case 1: /* start game */
	    if (uiscreen->data->gameindex)
		return loadgame (uiscreen);
	    else
		return preparegame (uiscreen);

	case 2: /* delete game */
	    if (uiscreen->data->gameindex &&
		uiscreen->confirm ("Really delete this game?"))
		deletegame (uiscreen);
	    break;

	case 3: /* exit game */
	    return STATE_QUIT;
	}
    }
}

/*----------------------------------------------------------------------
 * Top Level Function Definitions.
 */

/**
 * Construct the New Game Screen.
 * @param  game The game object.
 * @return      The new New Game Screen.
 */
UIScreen *new_NewGameScreen (Game *game)
{
    UIScreen *uiscreen; /* the screen to return */

    /* reserve memory for the screen and its data */
    uiscreen = new_UIScreen ();
    if (! (uiscreen->data = malloc (sizeof (UIScreenData)))) {
	uiscreen->destroy (uiscreen);
	return NULL;
    }

    /* initialise methods */
    uiscreen->init = init;
    uiscreen->show = show;

    /* get pointers to necessary modules */
    display = getdisplay ();
    controls = getcontrols ();
    config = getconfig ();

    /* initialise attributes */
    uiscreen->data->game = game;
    uiscreen->data->gameindex = 0;
    uiscreen->data->packindex = 0;
    uiscreen->data->highlight = 0;

    /* return the screen */
    return uiscreen;
}
