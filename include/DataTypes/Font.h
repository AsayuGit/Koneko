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

#ifndef _FONT_H
#define _FONT_H

    #include "Koneko.h"
    #include "Surface.h"
    #include "KON_Bitmap.h"

    typedef struct{
        KON_Surface* FontSurface;
        KON_Rect Rects[95]; /* Array containing one rect per printable ascii character */
    } BitmapFont;

    /*
        SUMMARY : Load a bitmap font from disk
        INPUT   : char* FilePath         : Path to the font to load.
        INPUT   : uint32_t FontColorKey  : The color key to apply to the font.
        OUTPUT  : BitmapFont*            : Pointer to the newly loaded font, or NULL on error.
    */
    BitmapFont* KON_LoadBitmapFont(char* FilePath, uint32_t FontColorKey);

    /*
        SUMMARY : Load a bitmap font from memory
        OUTPUT  : BitmapFont*            : Pointer to the newly loaded font, or NULL on error.
    */
    BitmapFont* KON_LoadBitmapFontFromMem(BITMAP* bitmap, uint32_t FontColorKey);

    /*
        SUMMARY : Free a previously loaded font
        INPUT   : BitmapFont* font : Pointer to the font to free
    */
    void KON_FreeBitmapFont(BitmapFont* font);

#endif