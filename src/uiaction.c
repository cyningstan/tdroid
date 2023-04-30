/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Action Screen User Interface Module.
 */

/*----------------------------------------------------------------------
 * Required Headers.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* compiler specific headers */
#include <i86.h>

/* project specific headers */
#include "uiscreen.h"
#include "tdroid.h"
#include "display.h"
#include "controls.h"
#include "config.h"
#include "game.h"
#include "level.h"
#include "robot.h"
#include "item.h"
#include "action.h"
#include "timer.h"
#include "fatal.h"


/*----------------------------------------------------------------------
 * Data Definitions.
 */

/**
 * @struct uiscreendata
 * Data for the deployment screen.
 */
struct uiscreendata {

    /** @var game The game data. */
    Game *game;

    /** @var level The state of the level in the current frame. */
    Level *level;

    /** @var robots The robots on the level, in priority order. */
    Robot **robots;

    /** @var robotcount The number of robots/guards on the level. */
    int robotcount;

    /** @var move The current move of the action. */
    int move;

    /** @var won 1 if the level is won, 0 if not. */
    int won;

    /** @var lost 1 if the level is lost, 0 if not. */
    int lost;

    /** @var playing 1 if the action is playing, 0 if paused. */
    int playing;

    /** @var beeped 1 if a beep has been played this move. */
    int beeped;

};

/**
 * @struct phaserbeam
 * The location of a phaser beam.
 */
typedef struct phaserbeam PhaserBeam;
struct phaserbeam {

    /** @var hit 1 when the phaser has hit something */
    int hit;

    /** @var x Current x coordinate. */
    int x;

    /** @var y Current y coordinate. */
    int y;

    /** @var xf X facing offset. */
    int xf;

    /** @var yf Y facing offset. */
    int yf;
};

/** @var display A pointer to the display module. */
static Display *display;

/** @var controls A pointer to the game controls module. */
static Controls *controls;

/** @var config A pointer to the configuration. */
static Config *config;

/** @var programmenu The menu for the programming screen. */
static char *actionmenu[] = {
    "Cancel menu",
    "Replay",
    "Done",
    "New game",
    "Exit game"
};

/** @var xoffset The x offset for each facing. */
static int xoffset[] = {
    0, /* north */
    +1, /* east */
    0, /* south */
    -1 /* west */
};

/** @var xoffset The x offset for each facing. */
static int yoffset[] = {
    -1, /* north */
    0, /* east */
    +1, /* south */
    0 /* west */
};

/*----------------------------------------------------------------------
 * Level 4 Function Definitions.
 */

/**
 * Do the shooting animations.
 * @param uiscreen The user interface screen.
 */
static void doshootinganimations (UIScreen *uiscreen, int move,
				  int phasercount)
{
    PhaserBeam *phaserbeams; /* the phaser beams */
    Robot **robots; /* pointer to list of robots */
    Level *level; /* pointer to level state */
    int r, /* robot count */
	p, /* phaser beam count */
	x, /* x location of robot */
	y, /* y location of robot */
	xf, /* x facing of robot */
	yf, /* y facing of robot */
	pos, /* position of beam in transit */
	allhit, /* all phaser beams have hit their target */
	blast; /* there is a blast to display */
    Timer *timer; /* a delay timer to control the animation */

    /* allocate memory for phaser beams */
    if (! (phaserbeams = malloc (phasercount * sizeof (PhaserBeam))))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* set convenience variables */
    robots = uiscreen->data->robots;
    level = uiscreen->data->level;

    /* initialise phaser beams */
    for (r = 0, p = 0; r < uiscreen->data->robotcount; ++r) {

	/* ingore dead robots and those not shooting */
	if (! robots[r])
	    continue;
	if (robots[r]->ram[move] != ACTION_SHOOT)
	    continue;

	/* work out robot coordinates */
	x = robots[r]->x;
	y = robots[r]->y;
	if ((! level->items[x + 16 * y] ||
	     level->items[x + 16 * y]->type != ITEM_PHASER) &&
	    ! robots[r]->hasphaser)
	    continue;
	xf = xoffset[robots[r]->facing];
	yf = yoffset[robots[r]->facing];

	/* ignore robots firing off the map */
	if (x + xf < 0 || x + xf > 15 || y + yf < 0 || y + yf > 11)
	    continue;

	/* start the phaser */
	phaserbeams[p].hit = 0;
	phaserbeams[p].x = x + xf;
	phaserbeams[p].y = y + yf;
	phaserbeams[p].xf = xf;
	phaserbeams[p].yf = yf;
	display->showphaserbeam (phaserbeams[p].x, phaserbeams[p].y,
				 abs (phaserbeams[p].xf));
	++p;

    }
    timer = new_Timer (125);
    /* put sound here */
    timer->wait (timer);

    /* start moving phaser beams */
    do {

	/* initialise */
	blast = 0;
	allhit = 1;
	timer = new_Timer (125);

	/* look at all the phaser bolts */
	for (p = 0; p < phasercount; ++p) {

	    /* ignore beams that have hit something */
	    if (phaserbeams[p].hit)
		continue;

	    /* remove the phaser beam */
	    pos = phaserbeams[p].x + 16 * phaserbeams[p].y;
	    display->hidephaserbeam (level, pos);

	    /* has the phaser beam hit something? */
	    if (level->cells[pos]->allowshoot ||
		level->items[pos] ||
		level->robots[pos]) {
		blast = phaserbeams[p].hit = 1;
		display->showblast (phaserbeams[p].x, phaserbeams[p].y);
		continue;
	    } else if (phaserbeams[p].x + phaserbeams[p].xf < 0 ||
		phaserbeams[p].x + phaserbeams[p].xf > 15 ||
		phaserbeams[p].y + phaserbeams[p].yf < 0 ||
		phaserbeams[p].y + phaserbeams[p].yf > 11) {
		phaserbeams[p].hit = 1;
		continue;
	    }

	    /* advance the phaser beam */
	    phaserbeams[p].x += phaserbeams[p].xf;
	    phaserbeams[p].y += phaserbeams[p].yf;
	    display->showphaserbeam (phaserbeams[p].x,
				     phaserbeams[p].y,
				     abs (phaserbeams[p].xf));
	    allhit = 0;

	}

	/* update the display */
	timer->wait (timer);

    } while (! allhit);

    /* erase all phaser beams */
    free (phaserbeams);
    display->showlevelmap (level);
    display->update ();
}

/**
 * Remove robots that are destroyed, and reset statuses of others.
 * @param  uiscreen The user interface screen.
 * @return          1 if any robots were destroyed.
 */
static void resetrobotstatuses (UIScreen *uiscreen)
{
    int r; /* robot counter */
    Level *level; /* the current level state */
    Robot **robots; /* pointer to the prioritised robots */

    /* set convenience variables */
    level = uiscreen->data->level;
    robots = uiscreen->data->robots;

    /* scan the level for robots */
    for (r = 0; r < uiscreen->data->robotcount; ++r)
	if (robots[r])
	    robots[r]->status = ROBOT_INERT;
}

/**
 * Remove robots that are destroyed, and reset statuses of others.
 * @param  uiscreen The user interface screen.
 * @return          1 if any robots were destroyed.
 */
static int checkdestroyedrobots (UIScreen *uiscreen)
{
    int r, /* robot counter */
	destroyed = 0; /* robots destroyed */
    Level *level; /* the current level state */
    Robot **robots; /* pointer to the prioritised robots */

    /* set convenience variables */
    level = uiscreen->data->level;
    robots = uiscreen->data->robots;

    /* scan the level for robots */
    for (r = 0; r < uiscreen->data->robotcount; ++r)
	if (! robots[r])
	    ; /* no robot here */
	else if (robots[r]->status == ROBOT_DESTROYED) {
	    display->showblast (robots[r]->x, robots[r]->y);
	    level->robots[robots[r]->x + 16 * robots[r]->y] = NULL;
	    robots[r]->destroy (robots[r]);
	    robots[r] = NULL;
	    destroyed = 1;
	}

    /* return 1 if robots were destroyed */
    return destroyed;
}

/**
 * Remove items that are destroyed, without resetting other statuses.
 * @param  level The level to scan for items.
 * @return       1 if any items were destroyed
 */
static int checkdestroyeditems (Level *level)
{
    int c, /* cell counter */
	destroyed = 0; /* 1 if items were destroyed */

    /* scan the map for items */
    for (c = 0; c < 192; ++c)
	if (! level->items[c])
	    ; /* no item here */
	else if (level->robots[c])
	    ; /* item is being carried */
	else if (level->items[c]->status == ITEM_DESTROYED) {
	    level->items[c]->destroy (level->items[c]);
	    level->items[c] = NULL;
	    display->showlevelmapsquare (level, c);
	    display->showblast (c % 16, c / 16);
	}

    /* return 1 if items were destroyed */
    return destroyed;
}

/**
 * Remove items that are dsetroyed, and reset statuses of others.
 * @param level The level to scan for items.
 */
static void resetitemstatuses (Level *level)
{
    int c; /* cell counter */
    for (c = 0; c < 192; ++c)
	if (! level->items[c])
	    ; /* no item here */
	else if (level->robots[c])
	    ; /* item is being carried */
	else if (level->items[c]->status == ITEM_DESTROYED) {
	    level->items[c]->destroy (level->items[c]);
	    level->items[c] = NULL;
	} else
	    level->items[c]->status = ITEM_INERT;
}

/*----------------------------------------------------------------------
 * Level 3 Function Definitions.
 */

/**
 * Play the first part of any sprint actions assigned to the robots.
 * @param uiscreen The user interface screen.
 * @param move     The move number.
 */
static int playsprintactions (UIScreen *uiscreen, int move)
{
    Robot **robots; /* pointer to prioritised robot list */
    Level *level; /* pointer to level state */
    Action *action; /* the action to perform */
    int r, /* robot counter */
	sprinting = 0, /* 1 if any robots are sprinting */
	destroyed = 0; /* 1 if anything was destroyed */
    Timer *timer; /* timer for blast noise */

    /* set convenience variables */
    robots = uiscreen->data->robots;
    level = uiscreen->data->level;

    /* execute any sprint actions */
    for (r = 0; r < uiscreen->data->robotcount; ++r)
	if (robots[r] &&
	    robots[r]->status != ROBOT_DESTROYED &&
	    robots[r]->ram[move] == ACTION_SPRINT) {
	    action = get_Action (ACTION_SPRINT);
	    action->execute (action, robots[r], level);
	    sprinting = 1;
	}

    /* check if anything was destroyed and let the player see the blasts */
    if (checkdestroyedrobots (uiscreen))
	destroyed = 1;
    if (checkdestroyeditems (uiscreen->data->level))
	destroyed = 1;
    if (destroyed) {
	timer = new_Timer (250);
	display->playsound (DISPLAY_NOISE_BLAST);
	uiscreen->data->beeped = 1;
	timer->wait (timer);
    }

    /* update the display if any sprint actions occurred */
    if (sprinting) {
	display->showlevelmap (level);
	display->update ();
	if (! uiscreen->data->beeped) {
	    display->playsound (DISPLAY_NOISE_MOVE);
	    uiscreen->data->beeped = 1;
	}
	delay (250);
    }

    /* tell calling process if there was any action */
    return sprinting;
}

/**
 * Play any action except shoot.
 * @param uiscreen The user interface screen.
 * @param move     The move number.
 */
static int playgeneralactions (UIScreen *uiscreen, int move)
{
    Robot **robots; /* pointer to prioritised robot list */
    Level *level; /* pointer to level state */
    Action *action; /* the action to perform */
    int r, /* robot counter */
	actionsdone = 0, /* 1 if any actions were performed */
	destroyed = 0; /* 1 if anything was destroyed */
    Timer *timer; /* timer for blast noise */

    /* set convenience variables */
    robots = uiscreen->data->robots;
    level = uiscreen->data->level;

    /* execute any non-shooting actions */
    for (r = 0; r < uiscreen->data->robotcount; ++r)
	if (robots[r] &&
	    robots[r]->status != ROBOT_DESTROYED &&
	    move < robots[r]->ramsize) {
	    if (robots[r]->ram[move] != ACTION_SHOOT &&
		robots[r]->ram[move] != ACTION_NONE) {
		actionsdone = 1;
		action = get_Action (robots[r]->ram[move]);
		action->execute (action, robots[r], level);
	    }
	}

    /* check if anything was destroyed and let the player see the blasts */
    if (checkdestroyedrobots (uiscreen))
	destroyed = 1;
    if (checkdestroyeditems (uiscreen->data->level))
	destroyed = 1;
    if (destroyed) {
	timer = new_Timer (250);
	display->playsound (DISPLAY_NOISE_BLAST);
	uiscreen->data->beeped = 1;
	timer->wait (timer);
    }

    /* update the display */
    if (actionsdone) {
	display->showlevelmap (level);
	display->update ();
	if (! uiscreen->data->beeped) {
	    display->playsound (DISPLAY_NOISE_MOVE);
	    uiscreen->data->beeped = 1;
	}
	delay (250);
    }

    /* tell calling process if there was any action */
    return actionsdone;
}

/**
 * Play shoot actions.
 * @param uiscreen The user interface screen.
 * @param move     The move number.
 */
static int playshootactions (UIScreen *uiscreen, int move)
{
    Robot **robots; /* pointer to prioritised robot list */
    Level *level; /* pointer to level state */
    Action *action; /* the action to perform */
    int r, /* robot counter */
	shooting = 0, /* 1 if any robots are shooting */
	destroyed = 0; /* 1 if anything was destroyed */
    Timer *timer; /* timer for blast noise */

    /* set convenience variables */
    robots = uiscreen->data->robots;
    level = uiscreen->data->level;

    /* execute any shooting actions */
    for (r = 0; r < uiscreen->data->robotcount; ++r)
	if (robots[r] &&
	    robots[r]->ram[move] == ACTION_SHOOT) {
	    action = get_Action (ACTION_SHOOT);
	    action->execute (action, robots[r], level);
	    ++shooting;
	}

    /* do the shooting animation */
    if (shooting) {
	display->playsound (DISPLAY_NOISE_PEWPEW);
	uiscreen->data->beeped = 1;
	doshootinganimations (uiscreen, move, shooting);
    }

    /* check if anything was destroyed and let the player see the blasts */
    if (checkdestroyedrobots (uiscreen))
	destroyed = 1;
    if (checkdestroyeditems (uiscreen->data->level))
	destroyed = 1;
    if (destroyed) {
	timer = new_Timer (250);
	display->playsound (DISPLAY_NOISE_BLAST);
	uiscreen->data->beeped = 1;
	timer->wait (timer);
    }

    /* reset the robot statuses */
    checkdestroyedrobots (uiscreen);
    resetrobotstatuses (uiscreen);

    /* update the display if any shooting actions occurred */
    if (shooting) {
	display->showlevelmap (level);
	display->update ();
	if (! uiscreen->data->beeped) {
	    display->playsound (DISPLAY_NOISE_MOVE);
	    uiscreen->data->beeped = 1;
	}
	delay (250);
    }

    /* tell calling process if there was any action */
    return shooting;
}

/**
 * Play any cell effects.
 * @param uiscreen The user interface screen.
 * @param move     The move number.
 * @param sprint   Effects for sprinting robots only.
 */
static int playeffects (UIScreen *uiscreen, int move, int sprint)
{
    int effects = 0, /* 1 if any effects happened */
	effect, /* 1 if the current cell had an effect */
	c, /* general counter */
	destroyed = 0, /* 1 if something was destroyed */
	teleport = 0; /* 1 if a teleport was activated */
    Level *level; /* a pointer to the level */
    Cell *cell; /* pointer to current cell */
    Robot *robot; /* pointer to a robot on a cell */
    Item *item; /* pointer to an item on a cell */
    Timer *timer; /* timer for any blast noise */

    /* initialise convenience variables */
    level = uiscreen->data->level;

    /* sweep through all of the cells */
    for (c = 0; c < 192; ++c) {
	cell = level->cells[c];
	robot = level->robots[c];
	if (sprint && robot && robot->ram[move] != ACTION_SPRINT)
	    continue;
	if (robot && move < robot->ramsize) {
	    effect = cell->onrobot (level, c % 16, c / 16);
	    if (effect && cell->type == CELL_TELEPORTER)
		teleport = 1;
	    effects |= effect;
	} else if ((item = level->items[c])) {
	    effect = cell->onitem (level, c % 16, c / 16);
	    if (effect && cell->type == CELL_TELEPORTER)
		teleport = 1;
	    effects |= effect;
	}
    }

    /* reset the robot statuses */
    if (checkdestroyedrobots (uiscreen))
	destroyed = 1;
    if (checkdestroyeditems (uiscreen->data->level))
	destroyed = 1;
    if (destroyed) {
 	timer = new_Timer (250);
	display->playsound (DISPLAY_NOISE_BLAST);
	uiscreen->data->beeped = 1;
	timer->wait (timer);
    } else if (teleport) {
 	timer = new_Timer (250);
	display->playsound (DISPLAY_NOISE_TELEPORT);
	uiscreen->data->beeped = 1;
	timer->wait (timer);
    }
    resetrobotstatuses (uiscreen);

    /* update the level and display */
    if (effects) {
	display->showlevelmap (level);
	display->update ();
	if (! uiscreen->data->beeped) {
	    display->playsound (DISPLAY_NOISE_MOVE);
	    uiscreen->data->beeped = 1;
	}
	delay (250);
    }

    /* return whether effects happened */
    return effects;
}

/*----------------------------------------------------------------------
 * Level 2 Function Definitions.
 */

/**
 * Play a single move of a turn.
 * @param uiscreen The user interface screen.
 * @param move     The move number.
 */
static void playmove (UIScreen *uiscreen, int move)
{
    Timer *timer; /* timer to ensure user sees actions */
    int actions = 0, /* 1 if there were any actions */
	effects = 0; /* 1 if there were any effects */

    /* start the timer */
    uiscreen->data->beeped = 0;
    timer = new_Timer (1000);

    /* action and effects from the sprinting phase */
    actions |= playsprintactions (uiscreen, move);
    effects |= playeffects (uiscreen, move, 1);

    /* actions and effects from the rest of the move */
    actions |= playgeneralactions (uiscreen, move);
    actions |= playshootactions (uiscreen, move);
    if (actions)
	delay (250);
    effects |= playeffects (uiscreen, move, 0);

    /* update the progress bar */
    display->showprogressbar (1 + move);
    display->update ();

    /* give the player time to see what happened */
    if (actions || effects)
	timer->wait (timer);
    else
	timer->destroy (timer);
}

/*----------------------------------------------------------------------
 * Level 1 Function Definitions.
 */

/**
 * Intialise the level replay.
 * @param uiscreen The user interface screen.
 */
static void initreplaylevel (UIScreen *uiscreen)
{
    Level *level, /* level in progress */
	*initial; /* pointer to initial level state */
    int c, /* cell counter */
	r, /* robot counter */
	s, /* swap robot number */
	seed = 0, /* random number seed */
	robotcount = 0; /* the number of robots */
    Robot **robots, /* pointer to prioritised robot list */
	*swaprobot; /* pointer to a robot to swap */

    /* clone the game level */
    initial = uiscreen->data->game->level;
    if (! (uiscreen->data->level = initial->clone (initial)))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
    level = uiscreen->data->level;

    /* initialise move counter */
    uiscreen->data->move = 0;
    uiscreen->data->playing = 1;

    /* build the robot list and set a seed for their priorities */
    for (c = 0; c < 192; ++c) {
	seed ^= level->cells[c]->type;
	if (level->robots[c])
	    ++robotcount;
    }
    robots = uiscreen->data->robots;
    if (! robots && ! (robots = malloc (robotcount * sizeof (Robot *))))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
    for (c = 0, r = 0; c < 192; ++c)
	if (level->robots[c])
	    robots[r++] = level->robots[c];

    /* scramble the robot priorities according to the seed */
    srand (seed);
    for (r = 0; r < robotcount; ++r) {
	s = rand () % robotcount;
	swaprobot = robots[r];
	robots[r] = robots[s];
	robots[s] = swaprobot;
    }

    /* put robot list in ui screen data */
    uiscreen->data->robots = robots;
    uiscreen->data->robotcount = robotcount;
}

/**
 * Play through the robots' actions.
 * @param uiscreen The user interface screen.
 */
static void playactions (UIScreen *uiscreen)
{
    int move; /* move counter */

    /* play through the moves */
    for (move = 0; move < 8; ++move)
	playmove (uiscreen, move);

    /* reset the item statuses */
    resetitemstatuses (uiscreen->data->level);
}

/**
 * Update the game level.
 * @param uiscreen The user interface screen.
 */
static void updatelevel (UIScreen *uiscreen)
{
    Game *game; /* pointer to the game */
    Level *level; /* pointer to the level */
    game = uiscreen->data->game;
    level = uiscreen->data->level;
    game->level->destroy (game->level);
    game->level = level->clone (level);
}

/*----------------------------------------------------------------------
 * Public Method Function Definitions.
 */

/**
 * Destroy the current UI screen when no longer needed.
 * @param uiscreen The screen to destroy.
 */
static void destroy (UIScreen *uiscreen)
{
    Level *level; /* pointer to level state */
    if (uiscreen) {
	if (uiscreen->data) {
	    if ((level = uiscreen->data->level))
		level->destroy (level);
	    if (uiscreen->data->robots)
		free (uiscreen->data->robots);
	    free (uiscreen->data);
	}
	free (uiscreen);
    }
}

/**
 * Initialise the UI state when first encountered.
 * @param uiscreen The screen to affect.
 */
static void init (UIScreen *uiscreen)
{
    uiscreen->data->game->state = STATE_ACTION;

    /* grab the initial level state */
    initreplaylevel (uiscreen);
}

/**
 * Show the UI screen.
 * @param uiscreen The screen to show.
 * @return         The ID of the screen to show next.
 */
static UIState show (UIScreen *uiscreen)
{
    int option; /* menu option chosen */
    Game *game; /* convenience pointer to the game */

    /* initialise convenience variables */
    game = uiscreen->data->game;

    /* initialise the display */
    display->showactionscreen (uiscreen->data->level);
    display->update ();

    /* action playback */
    playactions (uiscreen);

    /* main loop */
    while (1) {

	/* get a choice from the menu */
	while (! controls->fire ());
	option = display->menu (5, actionmenu, 2);
	switch (option) {

	case 0: /* cancel menu */
	    display->showprogressbar (8);
	    display->update ();
	    break;

	case 1: /* replay action */
	    display->showprogressbar (0);
	    uiscreen->data->level->destroy (uiscreen->data->level);
	    initreplaylevel (uiscreen);
	    display->showlevelmap (uiscreen->data->level);
	    display->update ();
	    delay (250);
	    playactions (uiscreen);
	    break;

	case 2: /* done with action */
	    display->showprogressbar (8);
	    display->update ();
	    updatelevel (uiscreen);
	    game->turn (game);
	    if (game->state == STATE_VICTORY)
		uiscreen->informwithnoise
		    ("You won the game!", DISPLAY_NOISE_VICTORY);
	    else if (game->state == STATE_COMPLETE)
		uiscreen->informwithnoise
		    ("You completed this level!",
		     DISPLAY_NOISE_COMPLETED);
	    else if (game->state == STATE_FAILED) {
		/* there is no failed screen, just a dialogue. */
		game->state = STATE_DEPLOY;
		uiscreen->informwithnoise
		    ("You failed to complete this level!",
		     DISPLAY_NOISE_FAILED);
	    }
	    return game->state;

	case 3: /* new game */
	    return STATE_NEWGAME;

	case 4: /* exit game */
	    return STATE_QUIT;

	}
    }
}

/*----------------------------------------------------------------------
 * Top Level Function Definitions.
 */

/**
 * Construct the Action Screen.
 * @param  game The game object.
 * @return      The new Action Screen.
 */
UIScreen *new_ActionScreen (Game *game)
{
    UIScreen *uiscreen; /* the screen to return */

    /* reserve memory for the screen and its data */
    uiscreen = new_UIScreen ();
    if (! (uiscreen->data = malloc (sizeof (UIScreenData)))) {
	uiscreen->destroy (uiscreen);
	return NULL;
    }

    /* initialise methods */
    uiscreen->destroy = destroy;
    uiscreen->init = init;
    uiscreen->show = show;

    /* get pointers to necessary modules */
    display = getdisplay ();
    controls = getcontrols ();
    config = getconfig ();

    /* initialise attributes */
    uiscreen->data->game = game;
    uiscreen->data->level = NULL;
    uiscreen->data->robots = NULL;
    uiscreen->data->robotcount = 0;
    uiscreen->data->move = 0;
    uiscreen->data->won = 0;
    uiscreen->data->lost = 0;
    uiscreen->data->playing = 0;

    /* return the screen */
    return uiscreen;
}

