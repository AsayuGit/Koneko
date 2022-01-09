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

    void     KON_RectToString(KON_Rect* rect, char* buffer, size_t buffLen);

#endif