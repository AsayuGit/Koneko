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

    KON_Rect KON_InitRect(int x, int y, int w, int h);
    KON_Rect KON_CatVectToRect(Vector2d* xy, Vector2d* wh);
    KON_Rect KON_RectPlusVector2i(KON_Rect* Rect, Vector2i* Vect); /* FIXME: present in CommunFunctions (Vector2d) */
    KON_Rect KON_RectMinusVector2i(KON_Rect* Rect, Vector2i* Vect);
    bool     KON_GetRectRectIntersection(KON_Rect* rectA, KON_Rect* rectB, KON_Rect* resultRect);
    void     KON_RectToString(KON_Rect* rect, char* buffer, size_t buffLen);

#endif