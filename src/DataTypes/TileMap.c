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
#include <linux/limits.h>

KON_Surface* KON_LoadBitMap(FILE* tileMapFile, char* rootDirectory) {
    KON_Surface* loadedSurface;
    uint32_t colorKey;
    char buffer[PATH_MAX];
    char path[PATH_MAX] = {0};
    
    fscanf(tileMapFile, "%x\n", &colorKey);

    fgets(buffer, PATH_MAX, tileMapFile);
    buffer[strcspn(buffer, "\n")] = '\0'; /* Remove trailing \n */

    strcat(path, rootDirectory);
    strcat(path, "/");
    strcat(path, buffer);

    if (!(loadedSurface = KON_LoadSurface(path, colorKey, SURFACE_KEYED)))
        return NULL;

    return loadedSurface;
}

TileMap* KON_LoadTileMap(FILE* tileMapFile, char* rootDirectory) {
    TileMap* loadedTilemap = NULL;
    unsigned int nbOfSolidTiles;
    unsigned int i, j;

    loadedTilemap = (TileMap*)malloc(sizeof(TileMap));

    /* Loading */
    loadedTilemap->tileSet = KON_LoadBitMap(tileMapFile, rootDirectory);

    /* Properties */
    fscanf(tileMapFile, "%u %u %u %u %u\n", &loadedTilemap->MapSizeX, &loadedTilemap->MapSizeY, &loadedTilemap->tMSizeX, &loadedTilemap->tMSizeY, &loadedTilemap->TileSize);

    /* LayerData */
    loadedTilemap->MapData = (unsigned int**)malloc(sizeof(unsigned int*)*loadedTilemap->MapSizeY);
    for (i = 0; i < loadedTilemap->MapSizeY; i++){
        loadedTilemap->MapData[i] = (unsigned int*)malloc(sizeof(unsigned int)*loadedTilemap->MapSizeX);
        for (j = 0; j < loadedTilemap->MapSizeX; j++){
            fscanf(tileMapFile, "%u", &loadedTilemap->MapData[i][j]);
        }
    }

    /* Solid tiles */
    fscanf(tileMapFile, "%u", &nbOfSolidTiles);
    loadedTilemap->solidTiles = NULL;
    for (i = 0; i < nbOfSolidTiles; i++){
        fscanf(tileMapFile, "%u", &j);
        KON_AppendToLinkedList(&loadedTilemap->solidTiles, &j, sizeof(unsigned int));
    }

    return loadedTilemap;
}

static void KON_DrawTile(MapLayer* Layer, TileMap* map, unsigned int TileID, unsigned int X, unsigned int Y) {
    /* Declaration */
    KON_Rect SrcTile, DstTile;
    unsigned int tileSize;

    tileSize = map->TileSize;

    /* Init */
    SrcTile.x = (TileID % map->tMSizeX) * tileSize;
    SrcTile.y = (TileID / map->tMSizeX) * tileSize;
    DstTile.x = X * tileSize + (int)(Layer->pos.x - Koneko.dDevice.Camera.x);
    DstTile.y = Y * tileSize + (int)(Layer->pos.y - Koneko.dDevice.Camera.y);

    SrcTile.w = SrcTile.h = DstTile.w = DstTile.h = tileSize;
    /* Logic */

    KON_DrawScaledSurfaceRect(map->tileSet, &SrcTile, &DstTile);
}

void KON_DrawTileMap(MapLayer* Layer) {
    /* Declaration */
    unsigned int i, j;
    TileMap* map;

    map = (TileMap*)Layer->layerData;

    /* Logic */
    if (Layer->shown && (Layer->layerType == KON_LAYER_TILEMAP)){
        for (i = 0; i < map->MapSizeY; i++){
            for (j = 0; j < map->MapSizeX; j++){
                KON_DrawTile(Layer, map, map->MapData[i][j], j, i);
            }
        }
    }
}

void KON_DrawBitMap(MapLayer* Layer) {
    Vector2d pos;

    pos.x = (int)(Layer->pos.x - Koneko.dDevice.Camera.x);
    pos.y = (int)(Layer->pos.y - Koneko.dDevice.Camera.y);

    KON_DrawSurface((KON_Surface*)Layer->layerData, &pos);
}

bool KON_GetTile(TileMap* tileMap, unsigned int X, unsigned int Y, unsigned int* tile) {
    if (X >= tileMap->MapSizeX || Y >= tileMap->MapSizeY)
        return false;
    *tile = tileMap->MapData[Y][X];
    return true;
}

bool KON_GetTileAtCoordinates(TileMap* tileMap, double X, double Y, unsigned int* tile) {
    unsigned int tileSize;

    tileSize = tileMap->TileSize;
    X /= tileSize;
    Y /= tileSize;

    return KON_GetTile(tileMap, X, Y, tile);
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

bool KON_IsTileMapTileSolid(TileMap* tileMap, unsigned int X, unsigned int Y, unsigned int* tile) {
    unsigned int fetchedTile;

    if (!KON_GetTile(tileMap, X, Y, &fetchedTile))
        return true;

    if (tile)
        *tile = fetchedTile;

    return KON_IsTileSolid(tileMap, fetchedTile);
}

/* Returns true if the Map Tile is solid */
bool KON_IsTileMapTileAtCoordinatesSolid(TileMap* tileMap, double X, double Y, unsigned int* tile) {
    unsigned int fetchedTile;

    if (!KON_GetTileAtCoordinates(tileMap, X, Y, &fetchedTile))
        return true;

    if (tile)
        *tile = fetchedTile;

    return KON_IsTileSolid(tileMap, fetchedTile);
}