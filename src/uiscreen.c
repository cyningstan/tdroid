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
#include <time.h>

/* project-specific headers */
#include "uiscreen.h"
#include "tdroid.h"
#include "fatal.h"
#include "display.h"
#include "controls.h"
#include "timer.h"


/*----------------------------------------------------------------------
 * Data Definitions.
 */

/** @var display A pointer to the display module. */
static Display *display;

/** @var controls A pointer to the game controls module. */
static Controls *controls;

/** @var noyesmenu A menu for the confirmation dialogue. */
static char *noyesmenu[] = {
    "No",
    "Yes"
};

/** @var okmenu A "menu" for the information dialogue. */
static char *okmenu[] = {
    "Ok"
};

/*----------------------------------------------------------------------
 * Public Method Function Definitions.
 */

/**
 * Destroy the current UI screen when no longer needed.
 * @param uiscreen The screen to destroy.
 */
static void destroy (UIScreen *uiscreen)
{
    if (uiscreen) {
	if (uiscreen->data)
	    free (uiscreen->data);
	free (uiscreen);
    }
}

/**
 * Initialise the UI state when first encountered.
 * @param uiscreen The screen to affect.
 */
static void init (UIScreen *uiscreen)
{
}

/**
 * Show the UI screen.
 * @param uiscreen The screen to show.
 * @return         The ID of the screen to show next.
 */
static UIState show (UIScreen *uiscreen)
{
    return STATE_QUIT;
}

/**
 * Confirm a potentially destructive action.
 * @param  message The message to display.
 * @return         1 if yes, 0 if no.
 */
static int confirm (char *message)
{
    int option; /* option selected */
    option = display->dialogue (message, 2, noyesmenu);
    while (controls->fire ());
    return option;
}

/**
 * Inform of an error or notice using a dialogue box.
 * @param message The message to display.
 */
static void inform (char *message)
{
    display->dialogue (message, 1, okmenu);
    while (controls->fire ());
}

/**
 * Inform of an error or notice using a dialogue box.
 * @param message The message to display.
 * @param noiseid The noise to play.
 */
static void informwithnoise (char *message, int noiseid)
{
    display->dialoguewithnoise (message, 1, okmenu, noiseid);
    while (controls->fire ());
}

/*----------------------------------------------------------------------
 * Top Level Function Definitions.
 */

/**
 * Construct a new generic UI screen. This won't work on its own but
 * sets up a few elements common to multiple user interface screens.It
 * will usually be called by one of the subclass constructors.
 */
UIScreen *new_UIScreen (void)
{
    UIScreen *uiscreen; /* the screen to return */

    /* reserve memory for the screen and its data */
    if (! (uiscreen = malloc (sizeof (UIScreen))))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* initialise methods */
    uiscreen->destroy = destroy;
    uiscreen->init = init;
    uiscreen->show = show;
    uiscreen->confirm = confirm;
    uiscreen->inform = inform;
    uiscreen->informwithnoise = informwithnoise;

    /* get pointers to necessary modules */
    display = getdisplay ();
    controls = getcontrols ();

    /* return the screen */
    return uiscreen;
}
