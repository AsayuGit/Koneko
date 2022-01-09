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

#ifndef _VECTOR_H
#define _VECTOR_H

    #include "Bool.h"
    #include <math.h>

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

    /*
        SUMMARY : Initialize a Vector2d with the provided values.
        INPUT   : double X, Y : The values to initialize the vector with.
        OUTPUT  : Vect2d vect : THe vector to initialize.
    */
    Vector2d KON_InitVector2d(double x, double y);

    /*
        SUMMARY : Initialize a Vector2i with the provided values.
        INPUT   : double X, Y : The values to initialize the vector with.
        OUTPUT  : Vect2d vect : The vector to initialize.
    */
    Vector2i KON_InitVector2i(int x, int y);

    /*
        SUMMARY : Compute the norm of a vector.
        INPUT   : Vector2x vect : The vectors to compute the norm of.
        OUTPUT  : double        : The norm of the passed vector.
    */
    #define   KON_GetVectNorm(vect) sqrt(vect.x * vect.x + vect.y * vect.y)

    /*
        SUMMARY : Compute the cross product of two vectors.
        INPUT   : Vect2x vect1, vect2 : The two vector to compute the cross product of.
        OUTPUT  : The cross product of the two vectors.
    */
    #define   KON_GetVectCrossProduct(vect1, vect2) ((vect1.x * vect2.y) - (vect1.y * vect2.x))
    
    /*
        SUMMARY : Compute the scalar product of a vector by a scalar.
        INPUT   : Vector2d vect :
        INPUT   : double scalar :
        OUTPUT  : Vector2d      :
    */
    Vector2d  KON_GetVectScalarProduct(Vector2d vect, double scalar);

    /*
        SUMMARY : Compute the scalar division of a vector by a scalar.
        INPUT   : Vector2d vect :
        INPUT   : double scalar :
        OUTPUT  : Vector2d      :
    */
    Vector2d  KON_GetVectScalarDivision(Vector2d vect, double scalar);

    /*
        SUMMARY : Computes the sum of two vectors.
        INPUT   : Vecto2d* vect1, *vect2 : The two vectors to add together.
        OUTPUT  : Vector2d               : The resulting vector.
    */
    Vector2d  KON_GetVectAddition(Vector2d* vect1, Vector2d* vect2);
    
    /*
        SUMMARY : Computes the addition of a vector by a scalar.
        INPUT   : Vector2d vect : The vector to add the scalar to.
        INPUT   : double scalar : The scalar to be added to the vector.
        OUTPUT  : Vector2d      : The resulting vector.
    */
    Vector2d  KON_GetVectScalarAddition(Vector2d vect, double scalar);

    /*
        SUMMARY : Compute the subtraction of a vector by a scalar.
        INPUT   : Vector2d vect : The vector to substract the scalar to.
        INPUT   : double scalar : The scalat to be substracted to the vector.
        OUTPUT  : Vector2d      : The resulting vector.
    */
    Vector2d  KON_GetVectScalarSubstraction(Vector2d vect, double scalar);

    /*
        SUMMARY : Compute the difference of two vector.
        INPUT   : Vecto2d* vect1, *vect2 : The two vectors to substract together.
        OUTPUT  : Vector2d               : The resulting vector.
    */
    Vector2d  KON_GetVectSubstraction(Vector2d* vect1, Vector2d* vect2);
    
    /*
        SUMMARY : Compute the vectorial product of two vector.
        INPUT   : Vecto2d* vect1, *vect2 : The two vectors to multiply together.
        OUTPUT  : Vector2d               : The resulting vector.
    */
    Vector2d  KON_GetVectProduct(Vector2d* vect1, Vector2d* vect2);

    /*
        SUMMARY : Copies a vector into another.
        INPUT   : Vect2x src  : The vector to be copied from.
        OUTPUT  : Vect2x dest : The vector to be copied into.
    */
    #define   KON_VectToVect(dest, src) { \
        dest.x = src.x;                   \
        dest.y = src.y;                   \
    }                                     \

    /*
        SUMMARY : Tells if there is an intersection between two segments, and return the point of intersection (if any).
        INPUT   : Vector2d* seg1Start, *seg1End : The first segment.
        INPUT   : Vector2d* seg2Start, *seg2End : The second segment.
        OUTPUT  : Vector2d* intersection        : The intersection between the two (if any).
    */
    bool      KON_GetVectIntersect(Vector2d* seg1Start, Vector2d* seg1End, Vector2d* seg2Start, Vector2d* seg2End, Vector2d* intersection);

    /*
        SUMMARY : The the aproximate direction of a vector.
        INPUT   : Vector2d vect : The vect to aproximate.
        OUTPUT  : Direction     : The aproximate direction of the vector.
    */
    Direction KON_GetDirectionFromVector(Vector2d vect);

#endif