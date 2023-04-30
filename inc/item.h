/*======================================================================
 * Team Droid: Jam Edition
 * A programming puzzle game with cute robots.
 *
 * Copyright (C) Damian Gareth Walker, 2022.
 * Item Header.
 */

/* types defined in this file */
typedef struct item Item;

#ifndef __ITEM_H__
#define __ITEM_H__

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/**
 * @enum ItemType
 * Identifiers for the six item types.
 */
typedef enum {
    ITEM_NONE, /* no item here */
    ITEM_SPAWNER, /* a robot spawner pseudo-item */
    ITEM_WALKER, /* a walker to allow lateral movement */
    ITEM_SPRING, /* a spring to allow leaping */
    ITEM_PHASER, /* a phaser to allow shooting */
    ITEM_CRATE, /* a crate that can block movement and shooting */
    ITEM_CARD /* a data card to win the game */
} ItemType;

/**
 * @enum ItemStatus
 * Item status - what has happened to the item in this move.
 */
typedef enum {
    ITEM_INERT, /* nothing has happened to the item */
    ITEM_PUSHED, /* the item has been pushed */
    ITEM_CONVEYED, /* the item has been conveyed this move */
    ITEM_TELEPORTED, /* the item has been teleported this move */
    ITEM_DESTROYED /* the item has been destroyed this move */
} ItemStatus;

/**
 * @enum Item
 * The attributes and methods of an item.
 */
struct item {

    /*
     * Attributes
     */

    /** @var type The item type. */
    int type;

    /** @var status The item status. */
    int status;

    /*
     * Methods
     */

    /**
     * Destroy the item when no longer needed.
     * @param item The item to destroy.
     */
    void (*destroy) (Item *item);

    /**
     * Create a clone of this item.
     * @param  item The item to clone.
     * @return      The new item.
     */
    Item *(*clone) (Item *item);

};

/*----------------------------------------------------------------------
 * Top Level Function Declarations.
 */

/**
 * Item constructor.
 * @param  type The type of item.
 * @return      The new item.
 */
Item *new_Item (int type);

#endif
