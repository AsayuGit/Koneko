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

#include "Types.h"

SDL_Rect KON_InitRect(int x, int y, int w, int h){
    SDL_Rect rect;
    
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    return rect;
}

SDL_Rect KON_CatVectToRect(Vector2d* xy, Vector2d* wh) {
    SDL_Rect rect;

    rect.x = xy->x;
    rect.y = xy->y;
    rect.w = wh->x;
    rect.h = wh->y;

    return rect;
}

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

SDL_Rect KON_RectPlusVector2i(SDL_Rect* Rect, Vector2i* Vect){
    SDL_Rect Result;

    Result.x = Rect->x + Vect->x;
    Result.y = Rect->y + Vect->y;
    Result.w = Rect->w;
    Result.h = Rect->h;

    return Result;
}

SDL_Rect KON_RectMinusVector2i(SDL_Rect* Rect, Vector2i* Vect){
    SDL_Rect Result;

    Result.x = Rect->x - Vect->x;
    Result.y = Rect->y - Vect->y;
    Result.w = Rect->w;
    Result.h = Rect->h;

    return Result;
}