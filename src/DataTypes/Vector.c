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

#include "Vector.h"

#define _GNU_SOURCE /* M_PI */
#include <math.h>

Vector2d KON_InitVector2d(double x, double y){
    Vector2d vector;

    vector.x = x;
    vector.y = y;

    return vector;
}

Vector2i KON_InitVector2i(int x, int y){
    Vector2i vector;

    vector.x = x;
    vector.y = y;

    return vector;
}

Vector2d KON_GetVectScalarProduct(Vector2d vect, double scalar) {
    vect.x *= scalar;
    vect.y *= scalar;

    return vect;
}

Vector2d KON_GetVectScalarDivision(Vector2d vect, double scalar) {
    vect.x /= scalar;
    vect.y /= scalar;

    return vect;
}

Vector2d KON_GetVectAddition(Vector2d vect1, Vector2d vect2){
    vect1.x += vect2.x;
    vect1.y += vect2.y;

    return vect1;
}

Vector2d KON_GetVectScalarAddition(Vector2d vect, double scalar){
    vect.x += scalar;
    vect.y += scalar;

    return vect;
}

Vector2d KON_GetVectScalarSubstraction(Vector2d vect, double scalar){
    vect.x -= scalar;
    vect.y -= scalar;

    return vect;
}

Vector2d KON_GetVectSubstraction(Vector2d vect1, Vector2d vect2){
    vect1.x -= vect2.x;
    vect1.y -= vect2.y;

    return vect1;
}

Vector2d KON_GetVectProduct(Vector2d vect1, Vector2d vect2) {
    vect1.x *= vect2.x;
    vect1.y *= vect2.y;

    return vect1;
}

bool KON_GetVectIntersect(Vector2d seg1Start, Vector2d seg1End, Vector2d seg2Start, Vector2d seg2End, Vector2d* intersection){
    Vector2d seg1Vect, seg2Vect, segStartDiff;
    double segVectCrossProduct, seg1IntersectPoint, seg2IntersectPoint;

    /* We compute the two segments's vectors */
    seg1Vect = KON_GetVectSubstraction(seg1End, seg1Start);
    seg2Vect = KON_GetVectSubstraction(seg2End, seg2Start);

    /* Then we compute the diff between the two vector's starting position */
    segStartDiff = KON_GetVectSubstraction(seg2Start, seg1Start);
    /* Followed by the cross product of the two */
    segVectCrossProduct = KON_GetVectCrossProduct(seg1Vect, seg2Vect);

    /* t = ( q - p ) × s / ( r × s ) */
    seg1IntersectPoint = KON_GetVectCrossProduct(segStartDiff, seg2Vect) / segVectCrossProduct;
    /* u = ( q - p ) × r / ( r × s ) */
    seg2IntersectPoint = KON_GetVectCrossProduct(segStartDiff, seg1Vect) / segVectCrossProduct;

    if ((seg1IntersectPoint >= 0 && seg1IntersectPoint <= 1) && (seg2IntersectPoint >= 0 && seg2IntersectPoint <= 1)){
        /* Segment intersection */
        if (intersection){
            /* p + t  r = q + u  s */
            *intersection = KON_GetVectAddition(KON_GetVectScalarProduct(seg1Vect, seg1IntersectPoint), seg1Start);
        }
        return true;
    }
    return false;
}

/*
    KON_GetDirectionFromVector() : Returns the direction corresponding to the vector's direction
    INPUT: Vector2d vect : Vector from which we want to know the direction
    OUTPUT: Direction :  : The direction corresponding to the vector vect
*/
Direction KON_GetDirectionFromVector(Vector2d vect) {
    const double piStep = M_PI / 8.0;
    double vectAngle;
    unsigned int i;
    
    vectAngle = atan2(vect.y, vect.x);
    if (vectAngle < 0)
        vectAngle += 2 * M_PI;

    for (i = 1; i < 16; i += 2) {
        if (vectAngle < piStep * i) {
            return (i - 1) / 2;
        }
    }

    return 0;
}