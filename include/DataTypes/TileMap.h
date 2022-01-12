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
        KON_Surface* tileSet;
        unsigned int TileSize;          /* Size (in pixel) of a tile */
        unsigned int tMSizeX;           /* Nb of colums in the tilemap surface */
        unsigned int tMSizeY;           /* Nb of rows in the tilemap surface */
        unsigned int MapSizeX;          /* Nb of tiles in the X direction */
        unsigned int MapSizeY;          /* Nb of tiles in the Y direction */
        unsigned int** MapData;         /* Map Data */
        LinkedList* solidTiles;               /* List Of tile to colide with */
    } TileMap;

    /* Load a tilemap from a map file */
    TileMap*     KON_LoadTileMap(FILE* tileMapFile, char* rootDirectory);

    void         KON_DrawTileMap(MapLayer* Layer);

    unsigned int KON_GetTile(TileMap* tileMap, unsigned int X, unsigned int Y);
    unsigned int KON_GetTileAtCoordinates(TileMap* tileMap, double X, double Y);
    bool         KON_IsTileSolid(TileMap* tileMap, unsigned int tile);
    bool         KON_IsTileMapTileSolid(TileMap* tileMap, unsigned int X, unsigned int Y);

    /*
        SUMMARY : Loads a bitmap from map file.
        INPUT   : FILE* titleMapFile     : Oppened tilemap file.
        INPUT   : char* rootDirectory    : Current Map's directory.
        OUTPUT  : KON_Surface*           : The loaded bitmap or NULL on error.
    */
    KON_Surface* KON_LoadBitMap(FILE* tileMapFile, char* rootDirectory); /* TODO: convert to sprite */

    /*
        SUMMARY : Draws a bitmap layer on screen.
        INPUT   : MapLayer* Layer : The bitmap layer to draw
    */
    void         KON_DrawBitMap(MapLayer* Layer);


#endif