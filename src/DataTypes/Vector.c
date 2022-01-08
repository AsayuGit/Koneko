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

#include "Vector.h"

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