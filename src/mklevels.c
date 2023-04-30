/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Main Program.
 */

/*----------------------------------------------------------------------
 * Headers
 */

/* ANSI C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* project-specific headers */
#include "levelpak.h"
#include "level.h"
#include "cell.h"
#include "item.h"
#include "robot.h"
#include "action.h"
#include "fatal.h"


/*----------------------------------------------------------------------
 * Constants.
 */

/** @const BUFSIZE is the maximum input buffer size */
#define BUFSIZE 1024

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/** @enum read_state is the state of the line-reading FSM */
typedef enum read_state {
    STATE_NONE, /* not reading anything in particular */
    STATE_LEVEL, /* reading a level */
    STATE_LAST /* placeholder */
} ReadState;

/** @var input is the input file handle. */
static FILE *input;

/** @var filename is the base filename without extension. */
static char *filename;

/** @var line is the number of the input line being processed */
static int line = 0;

/** @var levelpack is the level pack to build. */
static LevelPack *levelpack;

/** @var levl is the level being built. */
static Level *level;

/**
 * @var mapchars is an array of characters for map definitions.
 */
static char mapchars[20] = ".#^>v</\\%*+$GRWSPCD";

/** @var facingnames An array of the four direction names. */
static char *facingnames[4] = {
    "north",
    "east",
    "south",
    "west"
};

/** @var actionnames An array of the action names, and "wait". */
static char *actionnames[13] = {
    "wait",
    "stepforward",
    "stepbackward",
    "stepleft",
    "stepright",
    "sprint",
    "leap",
    "turnleft",
    "turnright",
    "turnabout",
    "take",
    "drop",
    "shoot"
};

/*----------------------------------------------------------------------
 * Level 3 Functions.
 */

/**
 * Get a number from the input line.
 * @return the number.
 */
static int getnumberfromline (void)
{
    /* local variables */
    char *token; /* token from the command line */

    /* grab the next token */
    if (! (token = strtok (NULL, " \r\n"))) {
	printf ("Number missing on line %d.\n", line);
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    }

    /* return the numeric value */
    return atoi (token);
}

/**
 * Get a facing from the input line.
 * @return the facing.
 */
static int facinglookup (void)
{
    char *token; /* token from the input line */
    int facing = -1, /* the facing identified */
	c; /* general counter */

    /* grab the next token */
    if (! (token = strtok (NULL, " \r\n"))) {
	printf ("Facing missing on line %d.\n", line);
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    }

    /* identify the facing */
    for (c = 0; c < 4; ++c)
	if (! strcmp (token, facingnames[c]))
	    facing = c;
    if (facing == -1) {
	printf ("Invalid facing %s.\n", token);
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    }

    /* return the facing */
    return facing;
}

/**
 * Get an action lookup from the input line.
 * @return the action ID.
 */
static int actionlookup (void)
{
    char *token; /* token from the input line */
    int action = -1, /* the action identified */
	c; /* general counter */

    /* grab the next token */
    if (! (token = strtok (NULL, " \r\n"))) {
	printf ("Action missing on line %d.\n", line);
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    }

    /* identify the action */
    for (c = 0; c <= 12; ++c)
	if (! strcmp (token, actionnames[c]))
	    action = c;
    if (action == -1) {
	printf ("Invalid action %s on line %d.\n", token, line);
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    }

    /* return the action */
    return action;
}

/*----------------------------------------------------------------------
 * Level 2 Functions.
 */

/**
 * Process a 'level' command.
 * @param state is the current state.
 * @return the state after processing.
 */
static ReadState processlevelline (ReadState state)
{
    /* local variables */
    int id; /* id of level */

    /* Create level */
    level = new_Level ();
    id = getnumberfromline ();
    levelpack->levels[id - 1] = level;

    /* deal with unrecognised command */
    return STATE_LEVEL;
}

/**
 * Process a 'name' command.
 */
static void processnameline (void)
{
    /* local variables */
    char *token; /* levelpack name token */

    /* get the name from the next token */
    if (! (token = strtok (NULL, "\r\n"))) {
	printf ("Levelpack name missing on line %d.\n", line);
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    }

    /* set the levelpack name */
    strcpy (levelpack->name, token);
}

/**
 * Process a 'map' command in a level section.
 */
static void processlevelmapline (void)
{
    /* local variables */
    int x, /* x coordinate of map sweep */
	y; /* y coordinate of map sweep */
    char *mapstr, /* map string token */
	*tptr; /* pointer to cell character */
    int t; /* cell index */

    /* get the map string */
    if (! (mapstr = strtok (NULL, "\r\n"))) {
	printf ("Map string missing on line %d.\n", line);
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    }
    if (strlen (mapstr) != 192) {
	printf ("Invalid map length %d (should be 192) on line %d.\n",
		strlen (mapstr), line);
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    }

    /* set the map terrain from the map string */
    for (x = 0; x < 16; ++x)
	for (y = 0; y < 12; ++y) {

	    /* look at and verify the map character */
	    if (! (tptr = strchr (mapchars, mapstr[x + 16 * y]))) {
		printf ("Invalid map character '%c' in line %d.\n",
			mapstr[x + 16 * y], line);
		fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
	    }
	    t = tptr - mapchars;

	    /* update the map accordingly */
	    switch (*tptr) {

	    case '.': /* simple cell characters */
	    case '#':
	    case '^':
	    case '>':
	    case 'v':
	    case '<':
	    case '/':
	    case '\\':
	    case '%':
	    case '*':
	    case '+':
	    case '$':
		level->cells[x + 16 * y] = get_Cell (t + 1);
		break;

	    case 'G': /* guard robot characters */
		level->cells[x + 16 * y] = get_Cell (CELL_FLOOR);
		level->robots[x + 16 * y] = new_Robot (ROBOT_GUARD);
		break;

	    case 'R': /* items and pseudo-items */
	    case 'D':
	    case 'W':
	    case 'S':
	    case 'P':
	    case 'C':
		level->cells[x + 16 * y] = get_Cell (CELL_FLOOR);
		level->items[x + 16 * y] = new_Item (t - 12);
		break;
	    }
	}
}

/**
 * Process a 'guard' command in a level section.
 */
static void processlevelguardline (void)
{
    int x, /* x coordinate */
	y, /* y coordinate */
	c; /* general counter */
    Robot *robot; /* convenience pointer to guard robot */

    /* grab and verify the coordinates */
    x = getnumberfromline ();
    y = getnumberfromline ();
    if (x < 0 || x >= 16 ||
	y < 0 || y >= 12) {
	printf ("Location %d,%d off 16x12 map.\n", x, y);
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    }

    /* ensure a robot exists there */
    if (! (robot = level->robots[x + 16 * y])) {
	printf ("No robot at %d,%d on line %d.\n", x, y, line);
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    }

    /* initialise the general values */
    strcpy (robot->name, "Guard");
    robot->ramsize = 8;
    robot->haswalker = 1;
    robot->hasspring = 1;
    robot->hasphaser = 1;
    robot->hasinventory = 1;
    robot->x = x;
    robot->y = y;

    /* read the facing */
    robot->facing = facinglookup ();

    /* read the commands */
    for (c = 0; c < 8; ++c)
	robot->ram[c] = actionlookup ();
}

/*----------------------------------------------------------------------
 * Level 1 Functions.
 */

/**
 * Initialise the command line options.
 * @param argc is the argument count.
 * @param argv is the array of arguments.
 */
static void initialiseoptions (int argc, char **argv)
{
    if (argc != 2)
	fatalerror (FATAL_COMMAND_LINE, __FILE__, __LINE__);
    filename = argv[1];
}

/**
 * Initialise the levelpack.
 */
static void initialiselevelpack (void)
{
    levelpack = new_LevelPack ();
}

/**
 * Open the input file.
 */
static void openinputfile (void)
{
    /* local variables */
    char *inputfilename;

    /* ascertain the filename */
    if (! (inputfilename = malloc (strlen (filename) + 5)))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
    sprintf (inputfilename, "%s.lvi", filename);

    /* attempt to open the input file */
    if (! (input = fopen (inputfilename, "r")))
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);

    /* clean up */
    free (inputfilename);
}

/**
 * Process a single input line.
 * @return 1 if a line was read, 0 if EOF was met.
 */
static int processinputline (void)
{
    /* local variables */
    char buf[BUFSIZE], /* input buffer */
	*pos, /* position of character we're searching for */
	*token; /* initial command token */
    static ReadState state = STATE_NONE; /* reading state */

    /* read a line into the buffer and validate it */
    if (! fgets (buf, BUFSIZE, input))
	return 0;
    ++line;
    if (! strchr (buf, '\n')) {
	printf ("Line %d too long.\n", line);
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    }

    /* filter out comments and blank lines */
    if ((pos = strchr (buf, ';')))
	*pos = '\n';
    if (*buf == '\n')
	return 1;

    /* tokenise the line */
    token = strtok (buf, " \r\n");
    
    /* process change-of-state commands */
    if (! strcmp (token, "level"))
	state = processlevelline (state);

    /* process level pack commands */
    else if (! strcmp (token, "name") && state == STATE_NONE)
	processnameline ();

    /* process the various level type commands */
    else if (! strcmp (token, "map") && state == STATE_LEVEL)
	processlevelmapline ();
    else if (! strcmp (token, "guard") && state == STATE_LEVEL)
	processlevelguardline ();

    /* process unrecognised commands */
    else {
	printf ("Unrecognised command: %s on line %d.\n", token, line);
	fatalerror (FATAL_INVALIDDATA, __FILE__, __LINE__);
    }

    /* tell the calling process that we didn't reach EOF */
    return 1;
}

/**
 * Close the input file.
 */
static void closeinputfile (void)
{
    fclose (input);
}

/**
 * Save the levelpack.
 */
static void savelevelpack (void)
{
    sprintf (levelpack->filename, "%s.lev", filename);
    if (! (levelpack->save (levelpack)))
	fatalerror (FATAL_NODATA, __FILE__, __LINE__);
}

/*----------------------------------------------------------------------
 * Top Level Function.
 */

/**
 * Main Program.
 * @param argc is the command line argument count.
 * @param argv is the array of command line arguments.
 * @return 0 on success, >0 on failure.
 */
int main (int argc, char **argv)
{
    /* initialisation */
    initialiseoptions (argc, argv);
    initialiselevelpack ();

    /* process input file */
    openinputfile ();
    while (processinputline ());
    closeinputfile ();

    /* complete and save the levelpack */
    savelevelpack ();

    /* clean up */
    levelpack->destroy (levelpack);
    return 0;
}
