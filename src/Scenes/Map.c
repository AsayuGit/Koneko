#include "Map.h"
#include "Koneko.h"
#include "TileMap.h"
#include "Entity.h"
#include "CommunFunctions.h"

#ifdef _XBOX
	#define _POSIX_
	#include <limits.h>
	#include <string.h>
#elif defined GEKKO
    #include <limits.h>
    #include <libgen.h>
#else
	#include <linux/limits.h>
	#include <libgen.h> /* dirname() */
#endif

#include <stdlib.h>
#include <string.h>

Map* KON_LoadMap(char* mapFilePath) {
    /* Declaration */
    Map* LoadedMap = NULL;
    MapLayer* currentLayer = NULL;
    FILE* MapFile = NULL;
    TileMap* loadedTileMap = NULL;
    Vector2d bdSize;
    KON_Renderers layerRenderer;
    unsigned int nbOfLayers, i;
    char filepath[PATH_MAX];
	#ifdef _XBOX
		char MapRoot[PATH_MAX];
	#else
		char* MapRoot = NULL;
	#endif

    /* Init */
    MapFile = fopen(mapFilePath, "r");
    if (!MapFile){
        KON_SystemMsg("(KON_LoadMap) Couldn't load map file: ", MESSAGE_ERROR, 1, mapFilePath);
        return NULL;
    }

    LoadedMap = (Map*)malloc(sizeof(Map));
    astrcpy(&LoadedMap->MapFilePath, mapFilePath);
    strcpy(filepath, mapFilePath);

#ifdef _XBOX
	{
		char MapPath[PATH_MAX];
		
		_splitpath(filepath, MapRoot, MapPath, NULL, NULL);
		strcat(MapRoot, MapPath);
	}
#else
    MapRoot = dirname(filepath);
#endif

    fscanf(MapFile, "%u", &nbOfLayers);
    LoadedMap->MapLayer = (MapLayer*)calloc(nbOfLayers, sizeof(MapLayer));
    LoadedMap->nbOfLayers = nbOfLayers;

    /* Logic */
    for (i = 0; i < nbOfLayers; i++){ /* For each layer */
        /* Check layer type */
        layerRenderer = 0;
        fscanf(MapFile, "%u\n", &layerRenderer);
        currentLayer = LoadedMap->MapLayer + i;
        switch (layerRenderer){
            case RENDER_2D_BITMAP:
                currentLayer->texture.gpuSide = (void*)KON_LoadBitMap(MapFile, MapRoot);

                KON_GetSurfaceSize((KON_Surface*)currentLayer->layerData, &bdSize);
                KON_InitRect(currentLayer->boundingBox, 0, 0, bdSize.x, bdSize.y);
                break;
            
            case RENDER_3D_RAYCAST:
                currentLayer->effectBufferPitch = Koneko.dDevice.InternalResolution.x * sizeof(uint32_t);
                currentLayer->zBufferPitch = Koneko.dDevice.InternalResolution.x * sizeof(double);
                currentLayer->effectBuffer = (uint32_t*)malloc(Koneko.dDevice.InternalResolution.y * currentLayer->effectBufferPitch);
                currentLayer->zBuffer = (double*)malloc(Koneko.dDevice.InternalResolution.y * currentLayer->zBufferPitch);
                /*
                currentLayer->effectTexture = SDL_CreateTexture(
                    Koneko.dDevice.Renderer, SDL_PIXELFORMAT_RGB888,
                    SDL_TEXTUREACCESS_STREAMING,
                    Koneko.dDevice.InternalResolution.x,
                    Koneko.dDevice.InternalResolution.y
                );*/

                {
                    KON_CPUSurface* surface = KON_LoadCPUBitMap(MapFile, MapRoot);
                    size_t surfaceSize;
                    
                    KON_GetCPUSurfaceSize(surface, &surfaceSize, &currentLayer->texture.cpuSide.width, &currentLayer->texture.cpuSide.height);
                    surfaceSize *= currentLayer->texture.cpuSide.height;

                    currentLayer->texture.cpuSide.pixelData = (uint32_t*)malloc(surfaceSize);
                    memcpy(currentLayer->texture.cpuSide.pixelData, KON_GetCPUSurfacePixelData(surface), surfaceSize);
                    KON_FreeCPUSurface(surface);
                }

                loadedTileMap = currentLayer->layerData = (void*)KON_LoadTileMap(MapFile, MapRoot);
                KON_InitRect(currentLayer->boundingBox, 0, 0, loadedTileMap->MapSizeX * loadedTileMap->TileSize, loadedTileMap->MapSizeY * loadedTileMap->TileSize);
                break;

            case RENDER_2D_TILEMAP:
                currentLayer->texture.gpuSide = KON_LoadBitMap(MapFile, MapRoot);
                loadedTileMap = currentLayer->layerData = (void*)KON_LoadTileMap(MapFile, MapRoot);
                KON_InitRect(currentLayer->boundingBox, 0, 0, loadedTileMap->MapSizeX * loadedTileMap->TileSize, loadedTileMap->MapSizeY * loadedTileMap->TileSize);
                break;

            default:
                KON_SystemMsg("(KON_LoadMap) unknown layer mode", MESSAGE_WARNING, 0);
                break;
        }
        LoadedMap->MapLayer[i].layerRenderer = layerRenderer;
        LoadedMap->MapLayer[i].shown = true;
        LoadedMap->MapLayer[i].displayList = KON_InitDisplayList();
    }

    /* free */
    if (MapFile)
        fclose(MapFile);

    return LoadedMap;
}

static void KON_UpdateLayerEntityPosition(MapLayer* layer) {
    LinkedList* entityInstanceList = layer->entityInstanceList;
    EntityInstance* entityInstance;

    while (entityInstanceList) {
        entityInstance = (EntityInstance*)entityInstanceList->data;
        KON_UpdateEntityPosition(entityInstance);
        entityInstanceList = entityInstanceList->next;
    }
}

void KON_AddSpriteToLayer(Map* map, Sprite* sprite, unsigned int layerID, unsigned int priority) {
    if (layerID >= map->nbOfLayers) {
        KON_SystemMsg("(KON_AddSpriteToLayer) No such layer !", MESSAGE_ERROR, 0);
        return;
    }

    KON_AddToDisplayList(&map->MapLayer[layerID].displayList, sprite, priority);
}

void KON_DrawMap(Map* map) {
    MapLayer* currentLayer;
    unsigned int i;
    
    /* Clear the screen before rendering a new frame */
    KON_ClearScreen();

    /* Draw each layer */
    /*for (i = map->nbOfLayers - 1; i >= 0; i--) {*/
    for (i = 0; i < map->nbOfLayers; i++) {
        currentLayer = map->MapLayer + i;

        /* Update all entities position */
        KON_UpdateLayerEntityPosition(currentLayer);

        KON_RenderLayer(currentLayer);
    }
}