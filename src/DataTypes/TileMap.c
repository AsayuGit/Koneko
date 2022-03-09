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

#include "TileMap.h"
#include "Map.h"
#include "Surface.h"
#include "Log.h"
#include "CommunFunctions.h"
#include "DisplayList.h"

#ifdef _XBOX
	#define _POSIX_
	#include <limits.h>
#else
	#include <linux/limits.h>
#endif

KON_Surface* KON_LoadBitMap(FILE* tileMapFile, char* rootDirectory) {
    KON_Surface* loadedSurface;
    uint32_t colorKey;
    char buffer[PATH_MAX];
    char path[PATH_MAX] = {0};
    
    fscanf(tileMapFile, "%x\n", &colorKey);

    fgets(buffer, PATH_MAX, tileMapFile);
    buffer[strcspn(buffer, "\n")] = '\0'; /* Remove trailing \n */

    strcat(path, rootDirectory);
	#ifndef _XBOX
		strcat(path, "/");
	#endif
    strcat(path, buffer);

    if (!(loadedSurface = KON_LoadSurface(path, colorKey, SURFACE_KEYED)))
        return NULL;

    return loadedSurface;
}

TileMap* KON_LoadTileMap(FILE* tileMapFile, char* rootDirectory) {
    TileMap* loadedTileMap = NULL;
    unsigned int nbOfSolidTiles, i, j;

    loadedTileMap = (TileMap*)malloc(sizeof(TileMap));

    /* Loading */
    loadedTileMap->tileSet = KON_LoadBitMap(tileMapFile, rootDirectory);

    /* Properties */
    fscanf(tileMapFile, "%u %u %u %u %u %u\n", &loadedTileMap->MapSizeX, &loadedTileMap->MapSizeY, &loadedTileMap->MapSizeZ, &loadedTileMap->tMSizeX, &loadedTileMap->tMSizeY, &loadedTileMap->TileSize);

    /* LayerData */
    loadedTileMap->MapDataSize = loadedTileMap->MapSizeX * loadedTileMap->MapSizeY * loadedTileMap->MapSizeZ;
    loadedTileMap->MapData = (unsigned int*)malloc(sizeof(unsigned int) * loadedTileMap->MapDataSize);
    for (i = 0; i < loadedTileMap->MapDataSize; i++) {
        fscanf(tileMapFile, "%u", &loadedTileMap->MapData[i]);
    }

    /* Solid tiles */
    fscanf(tileMapFile, "%u", &nbOfSolidTiles);
    loadedTileMap->solidTiles = NULL;
    for (i = 0; i < nbOfSolidTiles; i++){
        fscanf(tileMapFile, "%u", &j);
        KON_AppendToLinkedList(&loadedTileMap->solidTiles, &j, sizeof(unsigned int));
    }

    return loadedTileMap;
}


void KON_GetTileSrcRectInTileMap(TileMap* tileMap, unsigned int TileID, KON_Rect* tileRect) {
    tileRect->x = TileID % tileMap->tMSizeX * tileMap->TileSize;
    tileRect->y = TileID / tileMap->tMSizeX * tileMap->TileSize;
    tileRect->w = tileRect->h = tileMap->TileSize;
}

void KON_DrawTile(TileMap* tileMap, unsigned int TileID, Vector2d position) {
    KON_Rect SrcTile, DstTile;

    KON_GetTileSrcRectInTileMap(tileMap, TileID, &SrcTile);
    DstTile.x = position.x;
    DstTile.y = position.y;
    DstTile.w = DstTile.h = SrcTile.w;

    KON_DrawScaledSurfaceRect(tileMap->tileSet, &SrcTile, &DstTile);
}

void KON_DrawTileMap(MapLayer* Layer) {
    /* Declaration */
    unsigned int mapIndex;
    Vector2d tilePos;
    TileMap* map;

    map = (TileMap*)Layer->layerData;

    /* Logic */
    if (!Layer->shown || (Layer->layerRenderer != RENDER_2D_TILEMAP))
        return;

    while (mapIndex < map->MapDataSize) {
        for (tilePos.y = 0; tilePos.y < map->MapSizeY; tilePos.y++) {
            for (tilePos.x = 0; tilePos.x < map->MapSizeX; tilePos.x++) {
                KON_DrawTile(map, map->MapData[mapIndex], tilePos); /* TODO: KON_DrawTileAtCoords ?() */
                mapIndex++;
            }
        }
    }
}

void KON_DrawBitMap(MapLayer* Layer) {
    Vector2d pos;

    pos.x = (int)(Layer->pos.x - Koneko.dDevice.camera.position.x);
    pos.y = (int)(Layer->pos.y - Koneko.dDevice.camera.position.y);

    KON_DrawSurface((KON_Surface*)Layer->layerData, &pos);
}

bool KON_GetTile(TileMap* tileMap, unsigned int X, unsigned int Y, unsigned int Z, unsigned int* tile) {
    if (X >= tileMap->MapSizeX || Y >= tileMap->MapSizeY || Z >= tileMap->MapSizeZ)
        return false;
    *tile = tileMap->MapData[(Z * tileMap->MapSizeX * tileMap->MapSizeY) + (Y * tileMap->MapSizeY) + X];
    return true;
}

bool KON_GetTileAtCoordinates(TileMap* tileMap, double X, double Y, unsigned int Z, unsigned int* tile) {
    unsigned int tileSize;

    tileSize = tileMap->TileSize;
    X /= tileSize;
    Y /= tileSize;

    return KON_GetTile(tileMap, (unsigned int)X, (unsigned int)Y, Z, tile);
}

bool KON_IsTileSolid(TileMap* tileMap, unsigned int tile) {
    LinkedList* tileList = NULL;
    
    tileList = tileMap->solidTiles;
    while (tileList){
        /* If we find the tile amongst the list of solid tiles */
        if (tile == *(unsigned int*)(tileList->data))
            return true;
        tileList = tileList->next;
    }
    return false;
}

bool KON_IsTileMapTileSolid(TileMap* tileMap, unsigned int X, unsigned int Y, unsigned int Z, unsigned int* tile) {
    unsigned int fetchedTile;

    if (!KON_GetTile(tileMap, X, Y, Z, &fetchedTile))
        return true;

    if (tile)
        *tile = fetchedTile;

    return KON_IsTileSolid(tileMap, fetchedTile);
}

/* Returns true if the Map Tile is solid */
bool KON_IsTileMapTileAtCoordinatesSolid(TileMap* tileMap, double X, double Y, unsigned int Z, unsigned int* tile) {
    unsigned int fetchedTile;

    if (!KON_GetTileAtCoordinates(tileMap, X, Y, Z, &fetchedTile))
        return true;

    if (tile)
        *tile = fetchedTile;

    return KON_IsTileSolid(tileMap, fetchedTile);
}