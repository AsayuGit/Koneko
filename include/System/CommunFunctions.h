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

#ifndef _COMMUNFUNCTIONS_H
#define _COMMUNFUNCTIONS_H

    #include "Font.h"
    #include "Rect.h"
    #include "System.h"

    #define MAX(x, y) (((x) > (y)) ? (x) : (y))
    #define MIN(x, y) (((x) < (y)) ? (x) : (y))

    int      gputc(BitmapFont* Font, char c, unsigned int color, unsigned int x, unsigned int y);
    Vector2i gprintf(BitmapFont* Font, char* text, int intCharSpce, const KON_Rect* Bounds);
    Vector2i gstrlen(BitmapFont* Font, char* text, int intCharSpce);

    #define map(DstA, DstB, SrcA, SrcB, Value) {                                   \
        int mappedValue = DstA + ((DstB - DstA) / (SrcB - SrcA)) * (Value - SrcA); \
        return (mappedValue < DstB) ? mappedValue : DstB;                          \
    }                                                                              \

    /* Copy a source string to a newly allocated destination */
    char*    astrcpy(char** dst, char* src);
    KON_Rect RectToVieport(const KON_Rect* InputRect, const KON_Rect* ViewPort, const Vector2i* Position);
    void     BoundVect2iToRect(Vector2i* vect, KON_Rect rect);
    unsigned char KON_BoundVect2dToRect(Vector2d* vect, KON_Rect* rect);
    
    /* Bitwise Operations */
    void      KON_SetNegative(void* valuePointer, size_t dataSize);
    void      KON_SetPositive(void* valuePointer, size_t dataSize);

#endif