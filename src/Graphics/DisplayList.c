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

typedef struct {
    unsigned int priority;  /* The priority level of the layer */
    Sprite* sprite; /* Linked list of Sprite* */
} DisplayListItem;

DisplayList* KON_InitDisplayList() {
    return NULL;
}

void KON_FreeDisplayList(DisplayList* listToFree) {
    KON_ClearDisplayList(listToFree);
    free(listToFree);
}

void KON_AddToDisplayList(DisplayList** list, Sprite* sprite, unsigned int priority) {
    register LinkedList** listPointer = list;
    register DisplayListItem* item;

    while (*listPointer) { /* Search for the right priority */
        if (((DisplayListItem*)((*listPointer)->data))->priority >= priority)
            break;
        listPointer = &(*listPointer)->next;
    }

    item = (DisplayListItem*)calloc(1, sizeof(DisplayListItem));
    item->priority = priority;
    item->sprite = sprite;
    KON_InsertRefIntoLinkedList(listPointer, item);
}

void KON_RemoveFromDisplayList(DisplayList** list, Sprite* sprite) {
    LinkedList** listPointer = list;
    
    while (*listPointer) {
        DisplayListItem* item = (DisplayListItem*)(*listPointer)->data;
        if (item->sprite == sprite) {
            KON_DeleteLinkedListNode(listPointer);
            return;
        }

        listPointer = &(*listPointer)->next;
    }
}

void KON_ClearDisplayList(DisplayList* list) {
    KON_FreeLinkedList(&list);
}

void KON_DrawDisplayList(DisplayList* list) {
    register LinkedList* listIterator = list;

    /* Draw each sprite */
    while (listIterator) {
        KON_DrawSprite(((DisplayListItem*)listIterator->data)->sprite);
        listIterator = listIterator->next;
    }
}