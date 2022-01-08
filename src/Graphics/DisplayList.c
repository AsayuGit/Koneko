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

#include "DisplayList.h"

DisplayList* KON_AddSpriteToDisplayListAtIndex(DisplayList** list, Sprite* sprite, int index) {
    DisplayListItem newItem;

    if (!list)
        return NULL;

    newItem.index = index;
    newItem.sprite = sprite;

    /* We find where to instert the new DisplayListItem based on its index */
    while (*list) {
        if (((DisplayListItem*)(*list)->data)->index > index)
            break;

        list = &(*list)->next;
    }

    return KON_InsertIntoLinkedList(list, &newItem, sizeof(DisplayListItem));
}

DisplayList* KON_AddSpriteToDisplayList(DisplayList** list, Sprite* sprite) {
    return KON_AddSpriteToDisplayListAtIndex(list, sprite, 0);
}

void KON_RemoveItemFromDisplayList(DisplayList** list, DisplayListItem* item) {
    if (!list)
        return;
    
    while (*list){
        if (((DisplayListItem*)(*list)->data) == item) {
            KON_DeleteLinkedListNode(list);
            return;
        }
        list = &(*list)->next;
    }
}

void KON_SetDisplayListItemIndex(DisplayList** list, DisplayListItem* item, int index) {
    DisplayList** itemToMove;

    if (!list)
        return;
    
    /* Search for the Item in the displayList */
    while (*list) {
        if ((DisplayListItem*)(*list)->data == item) {
            itemToMove = list;
            break;
        }
    }

    /* Seach where to reinsert it */ /* FIXME: make that part into a function since its used at least twice ? */
    while (*list) {
        if (((DisplayListItem*)(*list)->data)->index > index)
            break;

        list = &(*list)->next;
    }

    KON_MoveItemIntoLinkedList(itemToMove, list);
}

void KON_FreeDisplayList(DisplayList** list) {
    KON_FreeLinkedList(list);
}
