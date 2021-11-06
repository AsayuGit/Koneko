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

#ifndef _MAP_H
#define _MAP_H

    #include "Types.h"

    enum {
        KON_LAYER_BITMAP = 1,
        KON_LAYER_TILEMAP
    }; /* TODO: KON_LayerType */

    typedef struct{
        char* bitMapPath;
        SDL_Texture* bitMapSurface;
        Vector2i bitMapSize;
        Uint32 colorKey;
    } BitMap;

    typedef struct{
        BitMap* tileSet;
        unsigned int TileSize;          /* Size (in pixel) of a tile */
        unsigned int tMSizeX;           /* Nb of colums in the tilemap surface */
        unsigned int tMSizeY;           /* Nb of rows in the tilemap surface */
        unsigned int MapSizeX;          /* Nb of tiles in the X direction */
        unsigned int MapSizeY;          /* Nb of tiles in the Y direction */
        unsigned int** MapData;         /* Map Data */
        LinkedList* solidTiles;               /* List Of tile to colide with */
    } TileMap;

    typedef struct{
        unsigned int layerType;
        void* layerData;
        Vector2d pos;
        SDL_Rect boundingBox;
        bool shown;
    } MapLayer;

    typedef struct{
        char* MapFilePath;
        unsigned int nbOfLayers;
        MapLayer* MapLayer;
    } Map;

#endif