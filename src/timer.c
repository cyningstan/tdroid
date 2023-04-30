/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Timer Module.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* compiler-specific headers */
#include <sys/timeb.h>

/* project headers */
#include "timer.h"
#include "fatal.h"


/*----------------------------------------------------------------------
 * Public Level Methods Definitions.
 */

/**
 * Destroy the timer without waiting for it to end.
 * @param timer The timer to destroy.
 */
static void destroy (Timer *timer)
{
    free (timer);
}

/**
 * Wait for the timer to complete, then destroy it.
 * @param timer The timer to destroy.
 */
static void wait (Timer *timer)
{
    struct timeb now; /* the current time */
    int remaining; /* time remaining */
    do {
	ftime (&now);
	remaining = (timer->end.time - now.time) * 1000 +
	    timer->end.millitm - now.millitm;
    } while (remaining > 0);
    free (timer);
}

/*----------------------------------------------------------------------
 * Top Level Function Definitions.
 */

/**
 * Create a new timer and set it going.
 * @param  milliseconds The length of the timer.
 * @return              The new timer.
 */
Timer *new_Timer (int milliseconds)
{
    Timer *timer; /* the new timer */

    /* reserve memory for timer */
    if (! (timer = malloc (sizeof (Timer))))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* initialise methods */
    timer->destroy = destroy;
    timer->wait = wait;

    /* initialise attributes */
    ftime (&timer->end);
    timer->end.millitm += milliseconds;
    while (timer->end.millitm > 1000) {
	++timer->end.time;
	timer->end.millitm -= 1000;
    }

    /* return the new timer */
    return timer;
}

