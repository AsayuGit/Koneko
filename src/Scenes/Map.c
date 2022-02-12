#include "Map.h"
#include "Koneko.h"
#include "TileMap.h"
#include "Entity.h"
#include "CommunFunctions.h"

#ifdef _XBOX
	#define _POSIX_
	#include <limits.h>
#else
	#include <linux/limits.h>
	#include <libgen.h> /* dirname() */
#endif

Map* KON_LoadMap(char* mapFilePath){
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
    MapFile = fopen(mapFilePath, "r");
    if (!MapFile){
        KON_SystemMsg("(KON_LoadMap) Couldn't load map file: ", MESSAGE_ERROR, 1, mapFilePath);
        return NULL;
    }

    LoadedMap = (Map*)malloc(sizeof(Map));
    astrcpy(&LoadedMap->MapFilePath, mapFilePath);
    strcpy(filepath, mapFilePath);

#ifdef _XBOX
	_splitpath(filepath, NULL, MapRoot, NULL, NULL);
#else
    MapRoot = dirname(filepath);
#endif

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
                KON_InitRect(currentLayer->boundingBox, 0, 0, bdSize.x, bdSize.y);
                break;
                
            case KON_LAYER_TILEMAP:
                loadedTileMap = currentLayer->layerData = (void*)KON_LoadTileMap(MapFile, MapRoot);
                KON_InitRect(currentLayer->boundingBox, 0, 0, loadedTileMap->MapSizeX * loadedTileMap->TileSize, loadedTileMap->MapSizeY * loadedTileMap->TileSize);
                break;

            default:
                KON_SystemMsg("(KON_LoadMap) unknown layer mode", MESSAGE_WARNING, 0);
                break;
        }
        LoadedMap->MapLayer[i].layerType = layerType;
        LoadedMap->MapLayer[i].shown = true;
        LoadedMap->MapLayer[i].displayList = KON_InitDisplayList();
    }

    /* free */
    if (MapFile)
        fclose(MapFile);

    return LoadedMap;
}

/* DODO : Move to own header */
static void KON_Draw2DLayer(MapLayer* layer) {
    switch (layer->layerType)
    {
        case KON_LAYER_BITMAP:
            KON_DrawBitMap(layer);
            break;

        case KON_LAYER_TILEMAP:
            KON_DrawTileMap(layer);
            break;
        
        default:
            break;
    }

    KON_DrawDisplayList(layer->displayList);
}

static void KON_Draw3DLayer(MapLayer* layer) {}

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

    KON_AddToDisplayList(map->MapLayer[layerID].displayList, sprite, priority);
}

void KON_DrawMap(Map* map) {
    MapLayer* currentLayer;
    unsigned int i;
    
    /* Clear the screen before rendering a new frame */
    SDL_RenderClear(Koneko.dDevice.Renderer);

    /* Draw each layer */
    /*for (i = map->nbOfLayers - 1; i >= 0; i--) {*/
    for (i = 0; i < map->nbOfLayers; i++) {
        currentLayer = map->MapLayer + i;

        /* Update all entities position */
        KON_UpdateLayerEntityPosition(currentLayer);

        switch (1) {
            case 1: /* 2D */
                KON_Draw2DLayer(currentLayer);
                break;

            case 2: /* Raycast */
                KON_Draw3DLayer(currentLayer);
                break;
                
            default:
                break;
        }
    }

    KON_FinishFrame(); /* Update the main window */
}