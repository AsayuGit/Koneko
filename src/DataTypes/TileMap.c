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
#include "Surface.h"
#include "Log.h"
#include "CommunFunctions.h"
#include <linux/limits.h>
#include <libgen.h> /* dirname() */


/*
    SUMMARY : Loads a bitmap from map file.
    INPUT   : FILE* titleMapFile     : Oppened tilemap file.
    INPUT   : char* rootDirectory    : Current Map's directory.
    OUTPUT  : KON_Surface*           : The loaded bitmap or NULL on error.
*/
static KON_Surface* KON_LoadBitMap(FILE* tileMapFile, char* rootDirectory) {
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

/* Load a tilemap from a map file */
TileMap* KON_LoadTileMap(FILE* tileMapFile, char* rootDirectory){
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

Map* KON_LoadMap(char* MapFilePath){
    /* Declaration */
    Map* LoadedMap = NULL;
    MapLayer* currentLayer = NULL;
    FILE* MapFile = NULL;
    TileMap* loadedTileMap = NULL;
    Vector2d bdSize;
    unsigned int nbOfLayers, layerType;
    unsigned int i;
    char* MapRoot = NULL;
    char filepath[PATH_MAX];

    /* Init */
    MapFile = fopen(MapFilePath, "r");
    if (!MapFile){
        KON_SystemMsg("(KON_LoadMap) Couldn't load map file: ", MESSAGE_ERROR, 1, MapFilePath);
        return NULL;
    }

    LoadedMap = (Map*)malloc(sizeof(Map));
    astrcpy(&LoadedMap->MapFilePath, MapFilePath);
    strcpy(filepath, MapFilePath);
    MapRoot = dirname(filepath);

    fscanf(MapFile, "%u", &nbOfLayers);
    LoadedMap->MapLayer = (MapLayer*)calloc(nbOfLayers, sizeof(MapLayer));
    LoadedMap->nbOfLayers = nbOfLayers;

    /* Logic */
    for (i = 0; i < nbOfLayers; i++){ /* For each layer */
        /* Check layer type */
        layerType = 0;
        fscanf(MapFile, "%u\n", &layerType);
        currentLayer = LoadedMap->MapLayer + i;
        switch (layerType){
            case KON_LAYER_BITMAP:
                currentLayer->layerData = (void*)KON_LoadBitMap(MapFile, MapRoot);

                KON_GetSurfaceSize((KON_Surface*)currentLayer->layerData, &bdSize);
                currentLayer->boundingBox = KON_InitRect(0, 0, bdSize.x, bdSize.y);
                break;
                
            case KON_LAYER_TILEMAP:
                loadedTileMap = currentLayer->layerData = (void*)KON_LoadTileMap(MapFile, MapRoot);
                currentLayer->boundingBox = KON_InitRect(0, 0, loadedTileMap->MapSizeX * loadedTileMap->TileSize, loadedTileMap->MapSizeY * loadedTileMap->TileSize);
                break;

            default:
                KON_SystemMsg("(KON_LoadMap) unknown layer mode", MESSAGE_WARNING, 0);
                break;
        }
        LoadedMap->MapLayer[i].layerType = layerType;
        LoadedMap->MapLayer[i].shown = true;
    }

    /* free */
    if (MapFile)
        fclose(MapFile);

    return LoadedMap;
}

/* FIXME: retreive colorKey*/
void KON_SaveTileMap(Map* MapToSave) {
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
        KON_SystemMsg("(KON_LoadMap) Couldn't load map file: ", MESSAGE_ERROR, 1, MapToSave->MapFilePath);
        return;
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
                /*fprintf(MapFile, "%x\n%s\n", ((SDL_Texture*)(currentMapLayer->layerData))->colorKey, ((BitMap*)(currentMapLayer->layerData))->bitMapPath);*/
                break;
            
            case KON_LAYER_TILEMAP:
                currentTileMap = ((TileMap*)(currentMapLayer->layerData));
                /*fprintf(MapFile, "%x\n%s\n", currentTileMap->tileSet->colorKey, currentTileMap->tileSet->bitMapPath);*/

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

    if (MapFile)
        fclose(MapFile);
}
