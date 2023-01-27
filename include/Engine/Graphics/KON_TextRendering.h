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

#ifndef _KON_TextRendering_H
#define _KON_TextRendering_H

#include "Font.h"
#include "KON_DisplayDevice.h"

/*
    SUMMARY: Writes a character from a font to a textbox
    INPUT  :
    OUTPUT : 
*/
int KON_PrintChar(BitmapFont* Font, KON_Surface* target, char c, unsigned int color, unsigned int x, unsigned int y, KON_Rect* boundingBox);

Vector2i KON_Print(BitmapFont* Font, KON_Surface* target, const char* text, const unsigned int x, const unsigned int y);

Vector2i KON_PrintTrim(BitmapFont* Font, KON_Surface* target, const char* text, const unsigned int x, const unsigned int y);

Vector2i KON_PrintEx(BitmapFont* Font, KON_Surface* target, const char* text, int intCharSpce, const bool skipWhiteSpace, const unsigned int x, const unsigned int y);

Vector2i KON_PrintLen(BitmapFont* Font, const char* text, int intCharSpce);

#endif
