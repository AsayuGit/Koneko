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
#include "LinkedList.h"

typedef struct {
    uint8_t flags;          /* How the layer should be processed */
    unsigned int priority;  /* The priority level of the layer */
    LinkedList* spriteList; /* Linked list of Sprite* */
} DisplayListLayer;

struct DisplayList {
    LinkedList* layers; /* LinkedList of DisplayListLayer */
};

static void KON_FreeDisplayListLayer(DisplayListLayer* layer) {
    KON_FreeLinkedList(&layer->spriteList);
    free(layer);
}

DisplayList* KON_InitDisplayList() {
    DisplayList* newDisplayList = (DisplayList*)calloc(1, sizeof(DisplayList));

    return newDisplayList;
}

void KON_FreeDisplayList(DisplayList* listToFree) {
    KON_ClearDisplayList(listToFree);
    free(listToFree);
}

void KON_AddToDisplayList(DisplayList* list, Sprite* sprite, unsigned int priority) {
    LinkedList** layerList = &list->layers;
    DisplayListLayer* spriteLayer;

    while (*layerList) { /* Search for the right priority layer */
        if (((DisplayListLayer*)(*layerList)->data)->priority >= priority)
            break;
        layerList = &(*layerList)->next;
    }

    if (!(*layerList)) { /* Allocate a new layer if necessary */
        spriteLayer = (DisplayListLayer*)calloc(1, sizeof(DisplayListLayer));
        spriteLayer->priority = priority;
        KON_InsertRefIntoLinkedList(layerList, spriteLayer);
    }

    /* Insert the sprite at the begining of its layer */
    spriteLayer = (DisplayListLayer*)(*layerList)->data;
    KON_InsertRefIntoLinkedList(&spriteLayer->spriteList, sprite);
}

void KON_RemoveFromDisplayList(DisplayList* list, Sprite* sprite) {
    LinkedList* layerList = list->layers;
    
    while (layerList) {
        DisplayListLayer* layer = (DisplayListLayer*)layerList->data;
        LinkedList** spriteList = &layer->spriteList;
        
        while (*spriteList) {
            if ((Sprite*)(*spriteList)->data == sprite) {
                KON_DeleteLinkedListNode(spriteList);
                return;
            }
            spriteList = &(*spriteList)->next;
        }

        layerList = layerList->next;
    }
}

void KON_ClearDisplayList(DisplayList* list) {
    LinkedList* layerList = list->layers;

    while (layerList) {
        KON_FreeDisplayListLayer((DisplayListLayer*)layerList->data);
        layerList = layerList->next;   
    }

    KON_FreeLinkedList(&list->layers);
    free(list);
}

void KON_DrawDisplayList(DisplayList* list) {
    LinkedList* layerList = list->layers;
    
    /* Draw each Layer */
    while (layerList) {
        DisplayListLayer* layer = (DisplayListLayer*)layerList->data;
        LinkedList* spriteList = layer->spriteList;
        
        /* Draw each Sprite in the layer */
        while (spriteList) {
            KON_DrawSprite((Sprite*)spriteList->data);
            spriteList = spriteList->next;
        }

        layerList = layerList->next;
    }
}