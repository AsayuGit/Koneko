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

#ifndef _GRAPHICS_H
#define _GRAPHICS_H

    #include "Scene.h"

    void            KON_DrawBitMap(MapLayer* Layer);
    void            KON_DrawTileMap(MapLayer* Layer);
    uint32_t        getpixel(SDL_Surface *surface, int x, int y);
    void            FlipBlitSurface(SDL_Surface* SourceSurface, const KON_Rect* SrcRect, SDL_Surface* ScreenTarget, const KON_Rect* DstRect, bool flipFlag);
    bool            RectOnRect(const KON_Rect* SrcRect, const KON_Rect* DstRect);
    bool            RectOnScreen(const KON_Rect* Rect);
    void            CenterCameraOnCoordinates(double X, double Y);
    void            BoundCameraToRegion(KON_Rect Region);
    unsigned int    KON_GetTile(SceneHandle* scene, unsigned int layerID, unsigned int X, unsigned int Y);
    unsigned int    KON_GetTileAtCoordinates(SceneHandle* scene, unsigned int layerID, double X, double Y);
    bool            KON_IsTileSolid(SceneHandle* scene, unsigned int layerID, unsigned int tile);
    bool            KON_IsMapTileSolid(SceneHandle* scene, unsigned int layerID, unsigned int X, unsigned int Y, unsigned int *tile);
    bool            KON_IsWorldTileSolid(SceneHandle* scene, unsigned int layerID, double X, double Y, unsigned int *tile);

#endif