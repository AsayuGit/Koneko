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

#ifndef _TILEMAP_H
#define _TILEMAP_H

    #include "Map.h"
    #include "Surface.h"
    #include "LinkedList.h"

    typedef struct{
        unsigned int TileSize;          /* Size (in pixel) of a tile */
        unsigned int tMSizeX;           /* Nb of colums in the tilemap surface */
        unsigned int tMSizeY;           /* Nb of rows in the tilemap surface */
        unsigned int MapSizeX;          /* Nb of tiles in the X direction */
        unsigned int MapSizeY;          /* Nb of tiles in the Y direction */
        unsigned int MapSizeZ;          /* Nb of tiles in the Z direction */
        size_t       MapDataSize;       /* Size of the allocated map array */
        unsigned int* MapData;          /* Map Data */
        LinkedList* solidTiles;               /* List Of tile to colide with */
    } TileMap;

    /* Load a tilemap from a map file */
    TileMap* KON_LoadTileMap(FILE* tileMapFile, char* rootDirectory);

    /*
        SUMMARY : Fetch the source rect of a tile in a tilemap.
        INPUT   : TileMap* tileMap    : The tilemap from which the source rect should be fetched.
        INPUT   : unsigned int TileID : The tile of whom we wanna know the source rect from.
        INPUT   : KON_Rect* tileRect  : The feched source rect.
    */
    void     KON_GetTileSrcRectInTileMap(TileMap* tileMap, unsigned int TileID, KON_Rect* tileRect);

    /*
        SUMMARY : Draw a tile on screen from a tilemap.
        INPUT   : KON_Surface* tileSheet : The surface containing the tiles
        INPUT   : TileMap* tileMap       : The tilemap from which the tile should be drawn.
        INPUT   : unsigned int TileID    : The tile which should be drawn.
        INPUT   : Vector2d position      : Where to draw the tile on screen (screen space).
    */
    void KON_DrawTile(KON_Surface* tileSheet, TileMap* tileMap, unsigned int TileID, Vector2d position);

    void     KON_DrawTileMap(MapLayer* Layer);

    bool     KON_GetTile(TileMap* tileMap, unsigned int X, unsigned int Y, unsigned int Z, unsigned int* tile);
    bool     KON_GetTileAtCoordinates(TileMap* tileMap, double X, double Y, unsigned int Z, unsigned int* tile);
    bool     KON_IsTileSolid(TileMap* tileMap, unsigned int tile);
    bool     KON_IsTileMapTileAtCoordinatesSolid(TileMap* tileMap, double X, double Y, unsigned int Z, unsigned int* tile);
    bool     KON_IsTileMapTileSolid(TileMap* tileMap, unsigned int X, unsigned int Y, unsigned int Z, unsigned int* tile);

    /*
        SUMMARY : Loads a bitmap from map file.
        INPUT   : FILE* titleMapFile     : Oppened tilemap file.
        INPUT   : char* rootDirectory    : Current Map's directory.
        OUTPUT  : KON_Surface*           : The loaded bitmap or NULL on error.
    */
    KON_Surface* KON_LoadBitMap(FILE* tileMapFile, char* rootDirectory); /* TODO: convert to sprite */
    KON_CPUSurface* KON_LoadCPUBitMap(FILE* tileMapFile, char* rootDirectory);

    /*
        SUMMARY : Draws a bitmap layer on screen.
        INPUT   : MapLayer* Layer : The bitmap layer to draw
    */
    void         KON_DrawBitMap(MapLayer* Layer);


#endif