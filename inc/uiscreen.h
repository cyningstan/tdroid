/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * User Interface Screen Header.
 */

/* UIScreen type definition */
typedef struct uiscreen UIScreen;

#ifndef __UISCREEN_H__
#define __UISCREEN_H__

/* header dependencies */
#include "game.h"

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/**
 * @enum UIState.
 * A list of user interface screens.
 */
typedef enum {
    STATE_NEWGAME, /* user is setting up a new game */
    STATE_SCORE, /* user is viewing high scores */
    STATE_DEPLOY, /* user is deploying robots */
    STATE_PROGRAM, /* user is programming robots */
    STATE_ACTION, /* user is watching actions unfold */
    STATE_COMPLETE, /* user is looking at the level completed screen */
    STATE_FAILED, /* user is looking at the level failed screen */
    STATE_VICTORY, /* user is looking at the victory screen */
    STATE_QUIT, /* user wants to leave the program */
    STATE_LAST /* placeholder */
} UIState;

/**
 * @struct UiScreen.
 * The user interface screen.
 */
typedef struct uiscreendata UIScreenData;
struct uiscreen {

    /*
     * Attributes
     */
    
    /** @var data The private UI data. */
    UIScreenData *data;

    /*
     * Methods
     */

    /**
     * Destroy the current UI screen when no longer needed.
     * @param uiscreen The screen to destroy.
     */
    void (*destroy) (UIScreen *uiscreen);

    /**
     * Initialise the UI state when first encountered.
     * @param uiscreen The screen to affect.
     */
    void (*init) (UIScreen *uiscreen);

    /**
     * Show the UI screen.
     * @param uiscreen The screen to show.
     * @return         The ID of the screen to show next.
     */
    UIState (*show) (UIScreen *uiscreen);

    /**
     * Confirm a potentially destructive action.
     * @param  message The message to display.
     * @return         1 if yes, 2 if no.
     */
    int (*confirm) (char *message);

    /**
     * Inform of an error or notice using a dialogue box.
     * @param message The message to display.
     */
    void (*inform) (char *message);

    /**
     * Inform of an error or notice using a dialogue box.
     * @param message The message to display.
     * @param noiseid The noise to play.
     */
    void (*informwithnoise) (char *message, int noiseid);

};

/*----------------------------------------------------------------------
 * Top Level Function Declarations.
 */

/**
 * Construct a new generic UI screen.
 * This won't work on its own but sets up a few elements
 * common to multiple user interface screens (such as
 * inventory browsing). It will usually be called
 * by one of the subclass constructors below.
 */
UIScreen *new_UIScreen (void);

/**
 * Construct the New Game Screen.
 * @param  game The game object.
 * @return      The new New Game Screen.
 */
UIScreen *new_NewGameScreen (Game *game);

/**
 * Construct the Best Scores Screen.
 * @param  game The game object.
 * @return      The new New Game Screen.
 */
UIScreen *new_ScoreScreen (Game *game);

/**
 * Construct the Deployment Screen.
 * @param  game The game object.
 * @return      The new Deployment Screen.
 */
UIScreen *new_DeploymentScreen (Game *game);

/**
 * Construct the Programming Screen.
 * @param  game The game object.
 * @return      The new Programming Screen.
 */
UIScreen *new_ProgrammingScreen (Game *game);

/**
 * Construct the Action Screen.
 * @param  game The game object.
 * @return      The new Action Screen.
 */
UIScreen *new_ActionScreen (Game *game);

/**
 * Construct the Level Complete Screen.
 * @param  game The game object.
 * @return      The new Level Complete  Screen.
 */
UIScreen *new_LevelCompleteScreen (Game *game);

/**
 * Construct the Level Failed Screen.
 * @param  game The game object.
 * @return      The new Level Failed Screen.
 */
UIScreen *new_LevelFailedScreen (Game *game);

/**
 * Construct the Victory Screen.
 * @param  game The game object.
 * @return      The new Victory Screen.
 */
UIScreen *new_VictoryScreen (Game *game);

#endif
