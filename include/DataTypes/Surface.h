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

#ifndef _SURFACE_H
#define _SURFACE_H

    #include "Types.h"

    enum {
        SURFACE_OPAQUE = 0,
        SURFACE_ALPHA = 1,
        SURFACE_KEYED = 2
    };

    typedef enum {
        DRAW_DEFAULT = 0,
        DRAW_HORIZONTAL_FLIP = 1,
        DRAW_VERTICAL_FLIP = 2,
        DRAW_NO_SCALE = 4
    } DrawFlags;

    typedef struct KON_Surface KON_Surface;

    KON_Surface* KON_LoadSurface(char* FilePath, DisplayDevice* Device, Uint32 ColorKey, Uint8 flags);
    SDL_Surface* KON_LoadCpuSurface(char* FilePath, DisplayDevice* Device, Uint32 ColorKey, Uint8 flags);
    void         KON_FreeSurface(KON_Surface* surface);

    KON_Surface* KON_CpuToGpuSurface(DisplayDevice* dDevice, SDL_Surface* cpuSurface);

    /*
        SUMMARY : When provided with a surface, allows the user to get its size.
        INPUT   : KON_Surface* surface : The surface to get the size from
        INPUT   : Vector2d* size       : The surface's size
    */
    void         KON_GetSurfaceSize(KON_Surface* surface, Vector2d* size);

    #define      KON_DrawScaledSurfaceRect(dDevice, surface, rect, dest) KON_DrawScaledSurfaceRectEx(dDevice, surface, rect, dest, DRAW_DEFAULT) 
    #define      KON_DrawSurfaceRect(dDevice, surface, rect, pos) KON_DrawSurfaceRectEx(dDevice, surface, rect, pos, DRAW_DEFAULT)
    #define      KON_DrawScaledSurface(dDevice, surface, dest) KON_DrawScaledSurfaceEx(dDevice, surface, dest, DRAW_DEFAULT)
    #define      KON_DrawSurface(dDevice, surface, pos) KON_DrawSurfaceEx(dDevice, surface, pos, DRAW_DEFAULT)

    void         KON_DrawScaledSurfaceRectEx(DisplayDevice* dDevice, KON_Surface* surface, SDL_Rect* rect, SDL_Rect* dest, DrawFlags flags);
    void         KON_DrawSurfaceRectEx(DisplayDevice* dDevice, KON_Surface* surface, SDL_Rect* rect, Vector2d* pos, DrawFlags flags);
    void         KON_DrawScaledSurfaceEx(DisplayDevice* dDevice, KON_Surface* surface, SDL_Rect* dest, DrawFlags flags);
    void         KON_DrawSurfaceEx(DisplayDevice* dDevice, KON_Surface* surface, Vector2d* pos, DrawFlags flags);


#endif