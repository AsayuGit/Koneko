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

#ifndef _VECTOR_H
#define _VECTOR_H

    #include "Bool.h"

    typedef struct{
        int x;
        int y;
    } Vector2i;

    typedef struct{
        double x;
        double y;
    } Vector2d;

    typedef enum {
        Right,
        DownRight,
        Down,
        DownLeft,
        Left,
        UpLeft,
        Up,
        UpRight
    } Direction;

    Vector2d KON_InitVector2d(double x, double y);
    Vector2i KON_InitVector2i(int x, int y);

    #define   KON_GetVectNorm(vect) sqrt(vect.x * vect.x + vect.y * vect.y)
    #define   KON_GetVectCrossProduct(vect1, vect2) ((vect1.x * vect2.y) - (vect1.y * vect2.x))
    
    Vector2d  KON_GetVectScalarProduct(Vector2d vect, double scalar);
    Vector2d  KON_GetVectScalarDivision(Vector2d vect, double scalar);
    Vector2d  KON_GetVectAddition(Vector2d vect1, Vector2d vect2);
    Vector2d  KON_GetVectScalarAddition(Vector2d vect, double scalar);
    Vector2d  KON_GetVectScalarSubstraction(Vector2d vect, double scalar);
    Vector2d  KON_GetVectSubstraction(Vector2d vect1, Vector2d vect2);
    Vector2d  KON_GetVectProduct(Vector2d vect1, Vector2d vect2);


    #define   KON_VectToVect(dest, src) { \
        dest.x = src.x;                   \
        dest.y = src.y;                   \
    }                                     \

    bool      KON_GetVectIntersect(Vector2d seg1Start, Vector2d seg1End, Vector2d seg2Start, Vector2d seg2End, Vector2d* intersection);
    Direction KON_GetDirectionFromVector(Vector2d vect);

#endif