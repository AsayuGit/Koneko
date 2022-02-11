#include "Map.h"
#include "Koneko.h"
#include "TileMap.h"
#include "Entity.h"
#include "CommunFunctions.h"
#include <linux/limits.h>
#include <libgen.h> /* dirname() */

Map* KON_LoadMap(char* mapFilePath) {
    /* Declaration */
    Map* LoadedMap = NULL;
    MapLayer* currentLayer = NULL;
    FILE* MapFile = NULL;
    TileMap* loadedTileMap = NULL;
    Vector2d bdSize;
    unsigned int nbOfLayers, i;
    KON_Renderers layerRenderer;
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
    MapRoot = dirname(filepath);

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
                currentLayer->layerData = (void*)KON_LoadBitMap(MapFile, MapRoot);

                KON_GetSurfaceSize((KON_Surface*)currentLayer->layerData, &bdSize);
                KON_InitRect(currentLayer->boundingBox, 0, 0, bdSize.x, bdSize.y);
                break;
            
            case RENDER_2D_TILEMAP:
            case RENDER_3D_RAYCAST:
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

        KON_RenderLayer(currentLayer);
        KON_DrawDisplayList(currentLayer->displayList);
    }

    KON_FinishFrame(); /* Update the main window */
}