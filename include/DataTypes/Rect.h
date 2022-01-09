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

#ifndef _RECT_H
#define _RECT_H

    #include "Vector.h"
    #include "Bool.h"

    #include <stddef.h>

    typedef struct {
        int x;
        int y;
        int w;
        int h;
    } KON_Rect;

    /*
        SUMMARY : Initialise a KON_Rect with the provided values.
        INPUT : int X, Y, W, H : The values to set the rect to.
    */
    #define KON_InitRect(rect, X, Y, W, H) { \
        rect.x = X;                          \
        rect.y = Y;                          \
        rect.w = W;                          \
        rect.h = H;                          \
    }                                        \

    /*
        SUMMARY : Concatenates two vects into a Rect.
        INPUT   : Vector2X xy, wh : The vectors to cat from.
        OUTPUT  : KON_Rect rect   : The rect to cat the values into.
    */
    #define KON_CatVectToRect(rect, xy, wh) { \
        rect.x = xy.x; rect.y = xy.y;         \
        rect.w = wh.x; rect.h = wh.y;         \
    }                                         \

    /*
        SUMMARY : Adds a vect to the upper part of a rect.
        INPUT   : KON_Rect rect   : The rect to add to.
        INPUT   : Vect2X vect     : The vect to add.
        OUTPUT  : KON_Rect result : The resulting rect.
    */
    #define KON_RectPlusVect(result, rect, vect) { \
        result.x = rect.x + vect.x;                \
        result.y = rect.y + vect.y;                \
        result.w = rect.w;                         \
        result.h = rect.h;                         \
    }                                              \

    /*
        SUMMARY : Tells if two rects are intersecting and is so return the intersecting aera.
        INPUT   : KON_Rect* rectA, *rectB : The Rects to get the intersection from.
        OUTPUT  : KON_Rect* resultRect    : The resulting intersection aera (if any).
    */
    bool    KON_GetRectRectIntersection(KON_Rect* rectA, KON_Rect* rectB, KON_Rect* resultRect);

    /*
        SUMMARY : Tells is a rect and a segment intersect and if so returns the point of intersection.
        INPUT   : KON_Rect rect             : The rect to compute the intersection with.
        INPUT   : Vector2d segStart, segEnd : The segment to compute the intersection aginst.
        OUTPUT  : Vector2d* intersection    : This resulting intersection point (if any).
    */
    bool    KON_GetRectVectIntersect(KON_Rect* rect, Vector2d* segStart, Vector2d* segEnd, Vector2d* intersection);

    /*
        SUMMARY : Write a string representation of a KON_Rect to a buffer of size buffLen.
        INPUT   : KON_Rect* rect : The rect to convert.
        INPUT   : size_t bufflen : The size of the buffer to write into.
        OUTPUT  : char* buffer   : The buffer to write into.
    */
    void    KON_RectToString(KON_Rect* rect, char* buffer, size_t buffLen);

#endif