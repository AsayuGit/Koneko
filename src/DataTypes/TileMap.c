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
#include "Log.h"
#include "CommunFunctions.h"
#include "DisplayList.h"

#include <stdlib.h>
#include <string.h>

#ifdef _XBOX
	#define _POSIX_
	#include <limits.h>
#elif defined(GEKKO)
    #include <limits.h>
#else
	#include <linux/limits.h>
#endif

TileMap* KON_LoadTileMap(MapLayer* layer, KON_XMLNode* layerNode) {
    const char* tileMapData = NULL, *solidTiles = NULL;
    char* buffer, *strtokPointer;
    unsigned int i, tile;
    KON_XMLNode* node, *tileMapNode;
    TileMap* loadedTileMap = NULL;

    /* Load each tileMap component */
    node = KON_GetXMLNodeChild(layerNode);
    while (node) {
        if (KON_CompareXMLNodeName(node, "tileMap")) {
            tileMapNode = node;
            tileMapData = KON_GetXMLNodeText(node);
        } else if (KON_CompareXMLNodeName(node, "solidTiles")) {
            solidTiles = KON_GetXMLNodeText(node);
        } else {
            return NULL;
        }

        node = KON_GetXMLNodeSibling(node);
    }
    if (!tileMapData || !solidTiles)
        return NULL;

    if (!(loadedTileMap = (TileMap*)malloc(sizeof(TileMap)))) {
        KON_SystemMsg("(KON_LoadTileMap) No more memory !", MESSAGE_ERROR, 0);
        return NULL;
    }

    loadedTileMap->MapSizeX = KON_GetXMLAttributeAsInt(tileMapNode, "X");
    loadedTileMap->MapSizeY = KON_GetXMLAttributeAsInt(tileMapNode, "Y");
    loadedTileMap->MapSizeZ = KON_GetXMLAttributeAsInt(tileMapNode, "Z");
    loadedTileMap->tMSizeX = KON_GetXMLAttributeAsInt(tileMapNode, "texX");
    loadedTileMap->tMSizeY = KON_GetXMLAttributeAsInt(tileMapNode, "texY");
    loadedTileMap->TileSize = KON_GetXMLAttributeAsInt(tileMapNode, "tileSize");

    /* Properties */
    loadedTileMap->MapDataLayerPitch = loadedTileMap->MapSizeX * loadedTileMap->MapSizeY;

    loadedTileMap->TileIndexSize = loadedTileMap->tMSizeX * loadedTileMap->tMSizeY;
    loadedTileMap->TileIndex = (Tile*)calloc(loadedTileMap->TileIndexSize, sizeof(Tile));

    /* LayerData */
    loadedTileMap->MapDataSize = loadedTileMap->MapDataLayerPitch * loadedTileMap->MapSizeZ;
    if (!(loadedTileMap->MapData = (unsigned int*)malloc(sizeof(unsigned int) * loadedTileMap->MapDataSize))) {
        KON_SystemMsg("(KON_LoadTileMap) No more memory !", MESSAGE_ERROR, 0);
        return NULL;
    }

    astrcpy(&buffer, tileMapData);
    strtokPointer = strtok(buffer, " \n");
    for (i = 0; i < loadedTileMap->MapDataSize; i++) {
        sscanf(strtokPointer, "%u", &loadedTileMap->MapData[i]);
        strtokPointer = strtok(NULL, " \n");
    }
    free(buffer);

    /* Solid tiles */
    astrcpy(&buffer, solidTiles);
    strtokPointer = strtok(buffer, " ");
    do {
        if (sscanf(strtokPointer, "%u", &tile))
            loadedTileMap->TileIndex[tile].isSolid = true;
    } while ((strtokPointer = strtok(NULL, " ")));
    free(buffer);

    return loadedTileMap;
}

void KON_DrawTile(KON_Surface* tileSheet, TileMap* tileMap, unsigned int TileID, Vector2d position) {
    KON_Rect SrcTile, DstTile;

    KON_GetTileSrcRectInTileMap((*tileMap), TileID, SrcTile);
    DstTile.x = position.x;
    DstTile.y = position.y;
    DstTile.w = DstTile.h = SrcTile.w;

    KON_DrawScaledSurfaceRect(tileSheet, &SrcTile, &DstTile);
}

void KON_DrawTileMap(MapLayer* Layer) {
    /* Declaration */
    unsigned int mapIndex, i, j;
    Vector2d tilePos = {0, 0};
    TileMap* map;

    map = (TileMap*)Layer->layerData;

    /* Logic */
    if (!Layer->shown || (Layer->layerRenderer != RENDER_2D_TILEMAP))
        return;

    mapIndex = 0;
    while (mapIndex < map->MapDataSize) {
        for (i = 0; i < map->MapSizeY; i++) {
            for (j = 0; j < map->MapSizeX; j++) {
                tilePos.y = i * map->TileSize;
                tilePos.x = j * map->TileSize;
                KON_DrawTile(Layer->texture.gpuSide, map, map->MapData[mapIndex], tilePos); /* TODO: KON_DrawTileAtCoords ?() */
                mapIndex++;
            }
        }
    }
}

void KON_DrawBitMap(MapLayer* Layer) {
    Vector2d pos;

    pos.x = (int)(Layer->pos.x - Koneko.dDevice.camera.position.x);
    pos.y = (int)(Layer->pos.y - Koneko.dDevice.camera.position.y);

    KON_DrawSurface(Layer->texture.gpuSide, &pos);
}


bool KON_GetTile(TileMap* tileMap, unsigned int X, unsigned int Y, unsigned int Z, unsigned int* tile) {
    if (X < tileMap->MapSizeX && Y < tileMap->MapSizeY && Z < tileMap->MapSizeZ) {
        *tile = tileMap->MapData[(Z * tileMap->MapDataLayerPitch) + (Y * tileMap->MapSizeX) + X];
        return true;
    } else {
        return false;
    }
}

bool KON_IsTileSolid(TileMap* tileMap, unsigned int tile) {
    return (tile < tileMap->TileIndexSize) ? tileMap->TileIndex[tile].isSolid : false;
}

bool KON_IsTileMapTileSolid(TileMap* tileMap, unsigned int X, unsigned int Y, unsigned int Z, unsigned int* tile) {
    unsigned int tileID;

    if (!KON_GetTile(tileMap, X, Y, Z, &tileID))
        return false;
    if (tile)
        *tile = tileID;
    return KON_IsTileSolid(tileMap, tileID);
}
