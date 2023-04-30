/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Graphical Display Handler Header.
 */

/* Types defined here */
typedef struct display Display;

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

/*----------------------------------------------------------------------
 * Required Includes.
 */

/* project headers */
#include "game.h"
#include "scoretbl.h"

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/**
 * @enum display_control is a control code.
 */
typedef enum display_control {
    DISPLAY_UP,
    DISPLAY_DOWN,
    DISPLAY_LEFT,
    DISPLAY_RIGHT,
    DISPLAY_FIRE
} DisplayControl;

/**
 * @enum display_sound Enumerates the sound effects.
 */
typedef enum display_sound {
    DISPLAY_NOISE_DEPLOY, /* robot deployment */
    DISPLAY_NOISE_MOVE, /* unit move in action screen */
    DISPLAY_NOISE_PEWPEW, /* unit firing laser */
    DISPLAY_NOISE_BLAST, /* something is destroyed */
    DISPLAY_NOISE_FORCEFIELD, /* forcefield deactivated */
    DISPLAY_NOISE_TELEPORT, /* something teleports */
    DISPLAY_NOISE_COMPLETED, /* level is completed */
    DISPLAY_NOISE_FAILED, /* level has failed */
    DISPLAY_NOISE_VICTORY /* game is won */
} DisplaySound;

/**
 * @struct display is the display handler.
 */
typedef struct display_data DisplayData;
struct display {

    /** @var data is the private display data */
    DisplayData *data;

    /**
     * Destroy the display when no longer needed.
     */
    void (*destroy) ();

    /**
     * Give display routines access to the game data.
     * @param ingame The game being played.
     */
    void (*setgame) (Game *ingame);

    /**
     * Update the screen from the buffer.
     */
    void (*update) (void);

    /**
     * Display a menu and get an option from it.
     * @param count   The number of options in the menu.
     * @param options The option names in an array.
     * @param initial The initial option selected.
     * @return        The number of the option selected.
     */
    int (*menu) (int count, char **options, int initial);

    /**
     * Show a dialogue box.
     * @param  message The message to display.
     * @param  count   The number of options.
     * @param  options The menu of buttons.
     * @return         The number of the option selected.
     */
    int (*dialogue) (char *message, int count, char **options);

    /**
     * Show a dialogue box.
     * @param  message The message to display.
     * @param  count   The number of options.
     * @param  options The menu of buttons.
     * @param  noiseid The noise to play.
     * @return         The number of the option selected.
     */
    int (*dialoguewithnoise) (char *message, int count, char **options, int noiseid);

    /**
     * Show a single square on the level map.
     * @param level    The level to show.
     * @param location The location to show.
     */
    void (*showlevelmapsquare) (Level *level, int location);

    /**
     * Show the present state of a level.
     * @param level The level to show.
     */
    void (*showlevelmap) (Level *level);

    /**
     * Make a sound.
     * @param id The ID of the sound.
     */
    void (*playsound) (int id);

    /**
     * Show the title screen.
     * @param display is the display to affect.
     */
    void (*showtitlescreen) ();

    /**
     * Await the fire key at the title screen.
     * @param display is the display to affect.
     */
    void (*titlekey) ();

    /**
     * Show the New Game screen.
     * @param name The display name of the game.
     * @param player The name of the player.
     * @param packname The name of the level pack.
     */
    void (*shownewgame) (char *name, char *player, char *packname);

    /**
     * Show a single line of the new game screen.
     * @param name      The display name of the game.
     * @param player    The name of the player.
     * @param packname  The name of the level pack.
     * @param line      The line to show.
     * @param highlight 1 if line should be highlighted.
     */
    void (*showgameoption) (char *game, char *player, char *packname,
			    int line, int highlight);

    /**
     * Allow editing of the player name.
     * @param name The name to edit.
     */
    void (*renameplayer) (char *name);

    /**
     * Show the Score screen.
     * @param title1    The first half of the screen title.
     * @param title2    The second half of the screen title.
     * @param packname  The name of the level pack.
     * @param scores    The score table to show.
     * @param levels    The number of levels to count.
     * @param highlight The line to highlight.
     */
    void (*showscorescreen) (char *title1, char *title2, char *packname,
			     ScoreTable *scores, int levels,
			     int highlight);

    /**
     * Show the Deployment Screen.
     * @param robots The robots still on the deployment panel.
     * @param level  The level to deploy robots on.
     */
    void (*showdeploymentscreen) (Robot **robots, Level *level);

    /**
     * Show a robot (or an empty space) on the deployment panel.
     * @params type    The type of robot 1..6.
     * @params present 1 if the robot is on the panel, 0 if not.
     */
    void (*showdeploymentrobot) (int type, int present);
    
    /**
     * Show the cursor on the deployment screen.
     * @param pos   The cursor position: <0 = robot panel, >=0 = map.
     * @param shown 1 if shown, 0 if hidden.
     */
    void (*deploymentcursor) (int pos, int shown);

    /**
     * Navigate the cursor around the deployment panel.
     * @param  cursor The initial cursor location.
     * @return        The final cursor location.
     */
    int (*navigatedeploypanel) (int cursor);

    /**
     * Navigate the cursor around the deployment panel.
     * @param  cursor The initial cursor location.
     * @return        The final cursor location.
     */
    int (*navigatedeploymap) (int cursor);

    /**
     * Show the current droid in the deployment screen.
     * @param robot The current droid.
     */
    void (*showrobottodeploy) (Robot *robot);

    /**
     * Show the programming screen.
     * @param  library The library of actions available this turn.
     * @param  robot   The current robot, or NULL if none.
     * @param  level   The level layout.
     */
    void (*showprogrammingscreen) (Action **library, Robot *robot,
				   Level *level);

    /**
     * Show the library on the programming screen.
     * @param  library The library of actions available.
     */
    void (*showlibrary) (Action **library);

    /**
     * Show the cursor on the program screen.
     * @param pos   The cursor position: <0 = robot panel, >=0 = map.
     * @param shown 1 if shown, 0 if hidden.
     */
    void (*programcursor) (int pos, int shown);

    /**
     * Navigate around the robot's RAM.
     * @param  cursor  The initial cursor location
     * @param  ramsize The size of the robot's RAM.
     * @return         The final cursor location.
     */
    int (*navigateram) (int cursor, int ramsize);

    /**
     * Navigate around the library.
     * @param cursor The initial cursor location.
     * @return       The final cursor location.
     */
    int (*navigatelibrary) (int cursor);

    /**
     * Navigate from the ROM.
     * @param cursor The initial cursor location.
     * @return       The final cursor location.
     */
    int (*navigaterom) (int cursor);

    /**
     * Navigate around the programming map.
     * @param cursor The initial cursor location.
     * @return       The final cursor location.
     */
    int (*navigateprogrammap) (int cursor);

    /**
     * Show the robot that's about to be programmed.
     * @param robot The robot selected.
     * @param level The level the robot is on.
     */
    void (*showrobottoprogram) (Robot *robot, Level *level);

    /**
     * Show the action screen.
     * @param  level   The level layout.
     */
    void (*showactionscreen) (Level *level);

    /**
     * Show a phaser beam in transit.
     * @param x The x position of the beam.
     * @param y The y position of the beam.
     * @param facing 0 for north/south, 1 for east/west.
     */
    void (*showphaserbeam) (int x, int y, int facing);

    /**
     * Hide a phaser beam by redisplaying the level map square
     * @param level    The level to show.
     * @param location The location to show.
     */
    void (*hidephaserbeam) (Level *level, int location);

    /**
     * Show a blast as a phaser beam hits something.
     * @param x The x position of the blast.
     * @param y The y position of the blast.
     */
    void (*showblast) (int x, int y);

    /**
     * Show a progress bar of x/8 steps.
     * @param count The number of steps already taken.
     */
    void (*showprogressbar) (int count);

};

/*----------------------------------------------------------------------
 * Constructor Declarations.
 */

/**
 * Display constructor.
 * @param colourset = 0 for mono, 1 for colour, 2 for nice colour.
 * @param quiet = 0 for sound and music, 1 for silence.
 * @return the new display.
 */
Display *new_Display (int colourset, int quiet);

#endif
