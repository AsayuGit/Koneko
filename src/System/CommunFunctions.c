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

#include "CommunFunctions.h"
#include "Log.h"

#include <stdlib.h>
#include <string.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

char* astrcpy(char** dst, char* src){
    size_t length;

    length = strlen(src)+1;                     /* First we compute how long is the input string */
    *dst = (char*)malloc(length*sizeof(char));   /* Then we allocate the right amount of memory */
    memcpy(*dst, src, length);                  /* Finally we copy the content from the source to the destination */

    return *dst;
}

KON_Rect RectToVieport(const KON_Rect* InputRect, const KON_Rect* ViewPort, const Vector2i* Position){
    KON_Rect Return;

    Return.x = InputRect->x + Position->x - ViewPort->x;
    Return.y = InputRect->y + Position->y - ViewPort->y;
    Return.w = InputRect->w;
    Return.h = InputRect->h;

    return Return;
}

Vector2d KON_Vector2iTo2d(Vector2i* vect){
    return KON_InitVector2d((double)vect->x, (double)vect->y);
}

Vector2i KON_Vector2dTo2i(Vector2d* vect){
    return KON_InitVector2i((int)vect->x, (int)vect->y);
}

unsigned char KON_BoundVect2iToRect(Vector2i* vect, KON_Rect* rect){
    unsigned char result;
    Vector2d vect2d = KON_Vector2iTo2d(vect);

    result = KON_BoundVect2dToRect(&vect2d, rect);
    *vect = KON_Vector2dTo2i(&vect2d);

    return result;
}

unsigned char KON_BoundVect2dToRect(Vector2d* vect, KON_Rect* rect){
    unsigned char result = 0;

    if (vect->x > rect->x + rect->w){
        vect->x = rect->x + rect->w;
        result |= 1;
    } else if (vect->x < rect->x){
        vect->x = rect->x;
        result |= 1;
    }

    if (vect->y > rect->y + rect->h){
        vect->y = rect->y + rect->h;
        result |= 2;
    } else if (vect->y < rect->y){
        vect->y = rect->y;
        result |= 2;
    }
    
    return result;
}

void KON_SetNegative(void* valuePointer, size_t dataSize) {
    *((uint64_t*)valuePointer) |= (1ul << ((dataSize * 8) - 1));
}

void KON_SetPositive(void* valuePointer, size_t dataSize) {
    *((uint64_t*)valuePointer) &= ~(1ul << ((dataSize * 8) - 1));
}

double KON_DegToRad(double angle) {
    return (angle * M_PI) / 180.0;
}