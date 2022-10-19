/*
    Koneko is a 2D/Raycast3D game engine written from scratch in ANSI C
    using SDL and libxml2. This engine is meant to allow game developpement
    for Linux, Windows and the og Xbox.

    Copyright (C) 2021-2022 Killian RAIMBAUD [Asayu] (killian.rai@gmail.com)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef _DISPLAYLIST_H
#define _DISPLAYLIST_H

    #include "Sprite.h"
    #include "LinkedList.h"

    typedef struct LinkedList DisplayList;

    /*
        SUMMARY : Frees a previously allocated DisplayList.
        INPUT   : DisplayList* listToFree : The DisplayList to be freed.
    */
    void         KON_FreeDisplayList(DisplayList* listToFree);

    /*
        SUMMARY : Add a Sprite to a DisplayList.
        INPUT   : DisplayList* list     : The DisplayList to add the Sprite to.
        INPUT   : Sprite* sprite        : The Sprite to add to the DisplayList.
        INPUT   : unsigned int priority : The priority level the Sprite should be drawn with (0 to inf / back to front).
    */
    void         KON_AddToDisplayList(DisplayList** list, Sprite* sprite, unsigned int priority);

    /*
        SUMMARY : Remove a previously added Sprite from a DisplayList.
        INPUT   : DisplayList* list : The DisplayList the Sprite should be removed from.
        INPUT   : Sprite* sprite    : The Sprite to be removed from the DisplayList.
    */
    void         KON_RemoveFromDisplayList(DisplayList** list, Sprite* sprite);

    /*
        SUMMARY : Clears a DisplayList of all its Sprites.
        INPUT   : DisplayList* list : The displayList to be cleared.
    */
    void         KON_ClearDisplayList(DisplayList* list);

    /*
        SUMMARY : Set a Sprite's priority in a DisplayList.
        INPUT   : DisplayList* list     : The DisplayList to set the Sprite's priority in.
        INPUT   : Sprite* sprite        : The Sprite to set the priority of.
        INPUT   : unsigned int priority : The priority level the Sprite should be drawn with (0 to inf / back to front).
    */
    #define      KON_SetSpritePrirority(list, sprite, priority) do { \
        KON_RemoveFromDisplayList(list, sprite);                     \
        KON_AddToDisplayList(list, sprite, priority);                \
    } while (0)                                                      \

    /*
        SUMMARY : Draws a DisplayList to the screen.
        INPUT   : DisplayList* list : The DisplayList to be drawn.
    */
    void         KON_DrawDisplayList(DisplayList* list);

    /*
        SUMMARY: Draws a DisplayList at a set location.
        INPUT   : DisplayList* list : The DisplayList to be drawn.
        INPUT   : double x, y       : Where to draw the DisplayList at.
    */
    void         KON_DrawDisplayListAt(DisplayList* list, double x, double y);

#endif