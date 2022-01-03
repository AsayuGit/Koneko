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

#include "Font.h"
#include "Surface.h"
#include "Graphics.h"

BitmapFont* KON_LoadBitmapFont(char FilePath[], DisplayDevice* DDevice, Uint32 FontColorKey) {
    /* Declaration */
    BitmapFont* LoadingFont;
    SDL_Surface* LoadingSurface;
    unsigned int FontPixX, FontPixY;
    unsigned int LetterIndex;
    int FontTexW, FontTexH;

    /* Init */
    LetterIndex = 0;
    FontPixX = 1;

    /* Load font surface*/
    LoadingFont = (BitmapFont*)malloc(sizeof(BitmapFont));
    LoadingSurface = KON_LoadCpuSurface(FilePath, DDevice, FontColorKey, SURFACE_KEYED);
    if (!LoadingSurface){
        printf("ERROR: (KON_LoadBitmapFont) Can't load font %s\n", SDL_GetError());
    }
    
    FontTexW = LoadingSurface->w;
    FontTexH = LoadingSurface->h;

    /* Fill-in rects for each letter */
    SDL_LockSurface(LoadingSurface); /* Ensure that the pixel data is available for hw surfaces */ /* VERRRY SLOW AND INEFICIENT */
    while ((FontPixX < FontTexW) && (LetterIndex < 95)){

        /* Get letter coordinates */
        FontPixY = 0;
        LoadingFont->Rects[LetterIndex].x = FontPixX;
        LoadingFont->Rects[LetterIndex].y = 1;

        /* Get letter width */
        while ((getpixel(LoadingSurface, FontPixX, FontPixY) != 0x0) && (FontPixX < FontTexW)){
            FontPixX++;
        }
        LoadingFont->Rects[LetterIndex].w = FontPixX - LoadingFont->Rects[LetterIndex].x;
        
        /* Get letter height */
        FontPixY = 1;
        while ((getpixel(LoadingSurface, FontPixX, FontPixY) != 0x0) && (FontPixY < FontTexH)){
            FontPixY++;
        }
        LoadingFont->Rects[LetterIndex].h = FontPixY - LoadingFont->Rects[LetterIndex].y;

        FontPixX++;
        LetterIndex++;
    }
    SDL_UnlockSurface(LoadingSurface);

    #ifdef _SDL
        LoadingFont->FontSurface = LoadingSurface;
    #else
        LoadingFont->FontSurface = SDL_CreateTextureFromSurface(DDevice->Renderer, LoadingSurface);
        SDL_FreeSurface(LoadingSurface);
    #endif

    return LoadingFont;
}