/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Item Header.
 */

/*----------------------------------------------------------------------
 * Required Headers.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* project specific headers */
#include "item.h"
#include "fatal.h"

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/*----------------------------------------------------------------------
 * Public Method Function Definitions.
 */

/**
 * Destroy the item when no longer needed.
 * @param item The item to destroy.
 */
static void destroy (Item *item)
{
    free (item);
}

/**
 * Create a clone of this item.
 * @param  item The item to clone.
 * @return      The new item.
 */
static Item *clone (Item *item)
{
    Item *newitem; /* the new item */
    if (! (newitem = new_Item (item->type)))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);
    newitem->status = item->status;
    return newitem;
}

/*----------------------------------------------------------------------
 * Top Level Function Declarations.
 */

/**
 * Item constructor.
 * @param  type The type of item.
 * @return      The new item.
 */
Item *new_Item (int type)
{
    Item *item; /* the new item */

    /* reserve memory for item */
    if (! (item = malloc (sizeof (Item))))
	fatalerror (FATAL_MEMORY, __FILE__, __LINE__);

    /* initialise attributes */
    item->type = type;
    item->status = ITEM_INERT;

    /* initialise methods */
    item->destroy = destroy;
    item->clone = clone;

    /* return the new item */
    return item;
}
