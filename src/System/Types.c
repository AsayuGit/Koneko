/*
    Koneko is a 2D/Raycast3D game engine written from scratch in ANSI C
    using SDL and libxml2. This engine is meant to allow game developpement
    for Linux, Windows and the og Xbox.

    Copyright (C) 2021 Killian RAIMBAUD [Asayu] (killian.rai@gmail.com)

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

#include "Types.h"

SDL_Rect InitRect(int x, int y, int w, int h){
    SDL_Rect rect;
    
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    return rect;
}

Vector2d InitVector2d(double x, double y){
    Vector2d vector;

    vector.x = x;
    vector.y = y;

    return vector;
}

Vector2i InitVector2i(int x, int y){
    Vector2i vector;

    vector.x = x;
    vector.y = y;

    return vector;
}

SDL_Rect RectPlusVector2i(SDL_Rect* Rect, Vector2i* Vect){
    SDL_Rect Result;

    Result.x = Rect->x + Vect->x;
    Result.y = Rect->y + Vect->y;
    Result.w = Rect->w;
    Result.h = Rect->h;

    return Result;
}

SDL_Rect RectMinusVector2i(SDL_Rect* Rect, Vector2i* Vect){
    SDL_Rect Result;

    Result.x = Rect->x - Vect->x;
    Result.y = Rect->y - Vect->y;
    Result.w = Rect->w;
    Result.h = Rect->h;

    return Result;
}

/* Add a new node at the end of the list pointer and returns its address */
Node* KON_appendToList(Node** List, void* newData, size_t dataSize){
    if (List){
        while (*List){
            List = (Node**)&(*List)->next;
        }
        (*List) = (Node*)malloc(sizeof(Node));
        (*List)->data = malloc(dataSize);
        (*List)->next = NULL;

        memcpy((*List)->data, newData, dataSize);

        return *List;
    }

    return NULL;
}

unsigned int KON_ListCount(Node* List){
    unsigned int nbOfElements = 0;

    while (List){
        nbOfElements++;
        List = (Node*)List->next;
    }

    return nbOfElements;
}

void KON_FreeList(Node** List){
    if (List){
        if (*List){
            if ((*List)->next){
                KON_FreeList((Node**)&(*List)->next);
            }
            free(*List);
            *List = NULL;
        }
    }
}

void KON_DeleteListNode(Node** node){
    Node* nextNode = (Node*)(*node)->next;
    free((*node));
    *node = nextNode;
}