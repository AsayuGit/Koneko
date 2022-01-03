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

#include "TileMap.h"
#include "Surface.h"
#include "CommunFunctions.h"
#include <linux/limits.h>
#include <libgen.h> /* dirname() */

/* Load a bitmap from a map file */
BitMap* KON_LoadBitMap(DisplayDevice* DDevice, FILE* bitMapFile, char* rootDirectory){
    BitMap* loadedBitmap;
    Uint32 colorKey;
    char Buffer[PATH_MAX];

    loadedBitmap = (BitMap*)malloc(sizeof(BitMap));

    fscanf(bitMapFile, "%x\n", &colorKey);
    fgets(Buffer, PATH_MAX, bitMapFile);
    Buffer[strcspn(Buffer, "\n")] = '\0';
    astrcpy(&loadedBitmap->bitMapPath, Buffer);
    loadedBitmap->bitMapSurface = KON_LoadSurface(strcat(strcat(strcpy(Buffer, rootDirectory), "/"), loadedBitmap->bitMapPath), DDevice, colorKey, SURFACE_KEYED);
    SDL_QueryTexture(loadedBitmap->bitMapSurface, NULL, NULL, &loadedBitmap->bitMapSize.x, &loadedBitmap->bitMapSize.y);
    loadedBitmap->colorKey = colorKey;

    return loadedBitmap;
}

/* Load a tilemap from a map file */
TileMap* KON_LoadTileMap(DisplayDevice* DDevice, FILE* tileMapFile, char* rootDirectory){
    TileMap* loadedTilemap = NULL;
    unsigned int nbOfSolidTiles;
    unsigned int i, j;

    loadedTilemap = (TileMap*)malloc(sizeof(TileMap));

    /* TileMap Surface */        
    loadedTilemap->tileSet = KON_LoadBitMap(DDevice, tileMapFile, rootDirectory);
    
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

Map* KON_LoadMap(DisplayDevice* DDevice, char* MapFilePath){
    /* Declaration */
    Map* LoadedMap = NULL;
    MapLayer* currentLayer = NULL;
    FILE* MapFile = NULL;
    TileMap* loadedTileMap = NULL;
    unsigned int nbOfLayers, layerType;
    unsigned int i;
    char* MapRoot = NULL;
    char filepath[PATH_MAX];

    /* Init */
    MapFile = fopen(MapFilePath, "r");
    if (!MapFile){
        printf("Couldn't load map file: %s !\n", MapFilePath);
        goto Error;
    }

    LoadedMap = (Map*)malloc(sizeof(Map));
    astrcpy(&LoadedMap->MapFilePath, MapFilePath);
    strcpy(filepath, MapFilePath);
    MapRoot = dirname(filepath);

    fscanf(MapFile, "%u", &nbOfLayers);
    LoadedMap->MapLayer = (MapLayer*)malloc(sizeof(MapLayer)*nbOfLayers);
    LoadedMap->nbOfLayers = nbOfLayers;

    /* Logic */
    for (i = 0; i < nbOfLayers; i++){ /* For each layer */
        /* Check layer type */
        layerType = 0;
        fscanf(MapFile, "%u\n", &layerType);
        currentLayer = LoadedMap->MapLayer + i;
        switch (layerType){
            case KON_LAYER_BITMAP:
                currentLayer->layerData = (void*)KON_LoadBitMap(DDevice, MapFile, MapRoot);
                currentLayer->boundingBox.w = ((BitMap*)currentLayer->layerData)->bitMapSize.x;
                currentLayer->boundingBox.h = ((BitMap*)currentLayer->layerData)->bitMapSize.y;
                currentLayer->boundingBox.x = currentLayer->boundingBox.y = 0;
                break;
            case KON_LAYER_TILEMAP:
                loadedTileMap = currentLayer->layerData = (void*)KON_LoadTileMap(DDevice, MapFile, MapRoot);
                currentLayer->boundingBox = KON_InitRect(0, 0, loadedTileMap->MapSizeX * loadedTileMap->TileSize, loadedTileMap->MapSizeY * loadedTileMap->TileSize);
                break;

            default:
                printf("ERROR: unknown layer mode\n");
                break;
        }
        LoadedMap->MapLayer[i].layerType = layerType;
        LoadedMap->MapLayer[i].shown = true;
    }

    /* free */
Error:
    if (MapFile)
        fclose(MapFile);

    return LoadedMap;
}

void KON_SaveTileMap(Map* MapToSave){
    /* Declaration */
    FILE* MapFile;
    MapLayer* currentMapLayer = NULL;
    LinkedList* FBTiles;
    unsigned int i, j, k;
    unsigned int nbOfLayers, nbOfSolidTiles = 0;
    TileMap* currentTileMap;

    /* Init */
    MapFile = fopen(MapToSave->MapFilePath, "w");
    if (!MapFile){
        printf("Couldn't load map file: %s !\n", MapToSave->MapFilePath);
        goto Error;
    }

    /* Logic */
    nbOfLayers = MapToSave->nbOfLayers;
    fprintf(MapFile, "%u\n", nbOfLayers);
    for (k = 0; k < nbOfLayers; k++){
        currentMapLayer = MapToSave->MapLayer + k;
        /* Print Layer header */

        fprintf(MapFile, "%u\n", currentMapLayer->layerType);
        switch (currentMapLayer->layerType)
        {
            case KON_LAYER_BITMAP:
                fprintf(MapFile, "%x\n%s\n", ((BitMap*)(currentMapLayer->layerData))->colorKey, ((BitMap*)(currentMapLayer->layerData))->bitMapPath);
                break;
            
            case KON_LAYER_TILEMAP:
                currentTileMap = ((TileMap*)(currentMapLayer->layerData));
                fprintf(MapFile, "%x\n%s\n", currentTileMap->tileSet->colorKey, currentTileMap->tileSet->bitMapPath);


                fprintf(MapFile, "%u %u %u %u %u\n", currentTileMap->MapSizeX, currentTileMap->MapSizeY,
                currentTileMap->tMSizeX, currentTileMap->tMSizeY, currentTileMap->TileSize);
                
                /* Print layer data */
                for (i = 0; i < currentTileMap->MapSizeY; i++){
                    for (j = 0; j < currentTileMap->MapSizeX; j++){
                        fprintf(MapFile, "%u ", currentTileMap->MapData[i][j]);
                    }
                    fprintf(MapFile, "\n");
                }

                /* Print solid tiles */
                FBTiles = currentTileMap->solidTiles;
                while (FBTiles){
                    nbOfSolidTiles++;
                    FBTiles = FBTiles->next;
                }
                fprintf(MapFile, "%u ", nbOfSolidTiles);
                FBTiles = currentTileMap->solidTiles;
                while (FBTiles){
                    fprintf(MapFile, "%u ", *(unsigned int*)FBTiles->data);
                    FBTiles = FBTiles->next;
                }
                break;

            default:
                break;
        }
        fprintf(MapFile, "\n");
    }

    /* free */
Error:
    if (MapFile)
        fclose(MapFile);
}
