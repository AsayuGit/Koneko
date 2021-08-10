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

#ifndef _GRAPHICS_H
#define _GRAPHICS_H

    #include "Scene.h"

    void            KON_DrawTile(DisplayDevice* DDevice, TileMap* WorldMap, unsigned int TileID, unsigned int X, unsigned int Y);
    void            KON_DrawTileMap(DisplayDevice* DDevice, TileMap* WorldMap);
    Uint32          getpixel(SDL_Surface *surface, int x, int y);
    SDL_Surface*    CreateEmptySurface(Uint32 flags, int Width, int Height);
    void            FlipBlitSurface(SDL_Surface* SourceSurface, const SDL_Rect* SrcRect, SDL_Surface* ScreenTarget, const SDL_Rect* DstRect, bool flipFlag);
    bool            RectOnRect(const SDL_Rect* SrcRect, const SDL_Rect* DstRect);
    bool            RectOnScreen(DisplayDevice* DDevice, const SDL_Rect* Rect);
    void            DrawFrame(DisplayDevice* DDevice);
    void            CenterCameraOnCoordinates(DisplayDevice* DDevice, unsigned int X, unsigned int Y);
    void            BoundCameraToRegion(DisplayDevice* DDevice, SDL_Rect Region);
    unsigned int    KON_GetTile(SceneHandle* scene, unsigned int layerID, unsigned int X, unsigned int Y);
    unsigned int    KON_GetTileAtCoordinates(SceneHandle* scene, unsigned int layerID, double X, double Y);
    bool            KON_IsTileSolid(SceneHandle* scene, unsigned int layerID, unsigned int tile);
    bool            KON_IsMapTileSolid(SceneHandle* scene, unsigned int layerID, unsigned int X, unsigned int Y, unsigned int *tile);
    bool            KON_IsWorldTileSolid(SceneHandle* scene, unsigned int layerID, double X, double Y, unsigned int *tile);

#endif