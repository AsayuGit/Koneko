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

#include "Rect.h"

#include <stdio.h>

bool KON_GetRectRectIntersection(KON_Rect* rectA, KON_Rect* rectB, KON_Rect* resultRect) {
    KON_Rect intersectRect = {0};
    register int difference;

    if ((difference = rectA->x - rectB->x) > 0) {
        intersectRect.x = rectA->x;
        intersectRect.w = (rectA->w < (difference = rectB->w - difference)) ? rectA->w : difference;
    } else {
        intersectRect.x = rectB->x;
        intersectRect.w = (rectB->w < (difference = rectA->w + difference)) ? rectB->w : difference;
    }

    if (intersectRect.w <= 0)
        return false;

    if ((difference = rectA->y - rectB->y) > 0) {
        intersectRect.y = rectA->y;
        intersectRect.h = (rectA->h < (difference = rectB->h - difference)) ? rectA->h : difference;
    } else {
        intersectRect.y = rectB->y;
        intersectRect.h = (rectB->h < (difference = rectA->h + difference)) ? rectB->h : difference;
    }

    if (intersectRect.h <= 0)
        return false;
    
    *resultRect = intersectRect;
    return true;
}

bool KON_GetRectVectIntersect(KON_Rect* rect, Vector2d* segStart, Vector2d* segEnd, Vector2d* intersection) {
    Vector2d tempSegStart, tempSegEnd;

    #define CheckIntersection(XS, YS, XE, YE) do {                                            \
        tempSegStart = KON_InitVector2d(XS, YS);                                              \
        tempSegEnd = KON_InitVector2d(XE, YE);                                                \
        if (KON_GetVectIntersect(segStart, segEnd, &tempSegStart, &tempSegEnd, intersection)) \
            return true;                                                                      \
    } while (0)                                                                               \

    CheckIntersection(rect->x, rect->y, rect->x, rect->y + rect->h); /* Left */
    CheckIntersection(rect->x + rect->w, rect->y, rect->x + rect->w, rect->y + rect->h); /* Right */
    CheckIntersection(rect->x, rect->y, rect->x + rect->w, rect->y); /* Up */
    CheckIntersection(rect->x, rect->y + rect->h, rect->x + rect->w, rect->y + rect->h); /* Down */

    return false;
}

/* TODO: Implement bufflen */
int KON_RectToString(KON_Rect* rect, char* buffer) {
    return sprintf(buffer, "{%d, %d, %d, %d}\n", rect->x, rect->y, rect->w, rect->h);
}

bool KON_coordInRect(int x, int y, KON_Rect* rect) {
    return (x >= rect->x && x <= rect->x + rect->w && y >= rect->y && y <= rect->y + rect->h);
}
