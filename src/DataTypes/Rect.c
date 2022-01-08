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

#include "Rect.h"
#include "API.h"

#include <stdio.h>

KON_Rect KON_InitRect(int x, int y, int w, int h){
    KON_Rect rect;
    
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    return rect;
}

KON_Rect KON_CatVectToRect(Vector2d* xy, Vector2d* wh) {
    KON_Rect rect;

    rect.x = xy->x;
    rect.y = xy->y;
    rect.w = wh->x;
    rect.h = wh->y;

    return rect;
}

#define KON_RectPlusVect(rect, vect, result) { \
    result.x = rect->x + vect->x;              \
    result.y = rect->y + vect->y;              \
    result.w = rect->w;                        \
    result.h = rect->h;                        \
}                                              \

KON_Rect KON_RectPlusVect2i(KON_Rect* rect, Vector2i* vect) {
    KON_Rect result;

    KON_RectPlusVect(rect, vect, result);
    return result;
}

KON_Rect KON_RectPlusVect2d(KON_Rect* rect, Vector2d* vect) {
    KON_Rect result;

    KON_RectPlusVect(rect, vect, result);
    return result;
}

bool KON_GetRectRectIntersection(KON_Rect* rectA, KON_Rect* rectB, KON_Rect* resultRect) {
    return SDL_IntersectRect((SDL_Rect*)rectA, (SDL_Rect*)rectB, (SDL_Rect*)resultRect);
}

bool KON_GetRectVectIntersect(KON_Rect rect, Vector2d segStart, Vector2d segEnd, Vector2d* intersection) {
    if (KON_GetVectIntersect(segStart, segEnd, KON_InitVector2d(rect.x, rect.y), KON_InitVector2d(rect.x, rect.y + rect.h), intersection)){ /* Left */
        return true;
    } else if (KON_GetVectIntersect(segStart, segEnd, KON_InitVector2d(rect.x + rect.w, rect.y), KON_InitVector2d(rect.x + rect.w, rect.y + rect.h), intersection)){ /* Right */
        return true;
    } else if (KON_GetVectIntersect(segStart, segEnd, KON_InitVector2d(rect.x, rect.y), KON_InitVector2d(rect.x + rect.w, rect.y), intersection)){ /* Up */
        return true;
    } else if (KON_GetVectIntersect(segStart, segEnd, KON_InitVector2d(rect.x, rect.y + rect.h), KON_InitVector2d(rect.x + rect.w, rect.y + rect.h), intersection)){ /* Down */
        return true;
    }
    return false;
}

void KON_RectToString(KON_Rect* rect, char* buffer, size_t buffLen) {
    sprintf(buffer, "{%d, %d, %d, %d}\n", rect->x, rect->y, rect->w, rect->h);
}