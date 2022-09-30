#include "Map.h"
#include "Koneko.h"
#include "TileMap.h"
#include "Entity.h"
#include "CommunFunctions.h"
#include "KON_FileSystem.h"

#ifdef _XBOX
    #define _POSIX_
    #include <limits.h>
#elif defined GEKKO
    #include <limits.h>
    #include <libgen.h> /* dirname() */
#else
    #include <linux/limits.h>
    #include <libgen.h> /* dirname() */
#endif

#include <stdlib.h>
#include <string.h>

static void KON_LoadLayerSurface(MapLayer* layer, KON_XMLNode* layerNode) {
    uint32_t colorKey;
    const char* colorKeyString, *surfacePath = KON_GetXMLAttribute(layerNode, "surfacePath");

    if ((colorKeyString = KON_GetXMLAttribute(layerNode, "colorKey"))) {
        sscanf(colorKeyString, "0x%x", &colorKey);
        layer->texture.gpuSide = KON_LoadSurface(surfacePath, colorKey, SURFACE_KEYED);
    } else {
        layer->texture.gpuSide = KON_LoadSurface(surfacePath, 0, SURFACE_ALPHA);
    }
}

static int KON_LoadMapLayer(MapLayer* layer, KON_XMLNode* layerNode) {
    const char* message, *layerType = KON_GetXMLAttribute(layerNode, "type");
    KON_XMLNode* layerProperty;
    bool loadTileMap = false;

    if (strcmp(layerType, "BitMap") == 0) {
        KON_LoadLayerSurface(layer, layerNode);
        layer->layerRenderer = RENDER_2D_BITMAP;
        message = "(KON_LoadMapLayer) Loaded NEW BitMap Layer";
    } else if (strcmp(layerType, "TileMap") == 0) {
        KON_LoadLayerSurface(layer, layerNode);
        layer->layerRenderer = RENDER_2D_TILEMAP;
        message = "(KON_LoadMapLayer) Loaded NEW TileMap Layer";
        loadTileMap = true;
    } else if (strcmp(layerType, "Raycast") == 0) {
        KON_LoadLayerSurface(layer, layerNode);
        layer->layerRenderer = RENDER_3D_RAYCAST;
        message = "(KON_LoadMapLayer) Loaded NEW Raycast Layer";
        loadTileMap = true;
    } else if (strcmp(layerType, "Empty") == 0) {
        layer->layerRenderer = RENDER_NONE;
        message = "(KON_LoadMapLayer) Loaded NEW Empty Layer";
    } else {
        return -1;
    }

    layerProperty = KON_GetXMLNodeChild(layerNode);
    while (layerProperty) {
        if (loadTileMap && KON_CompareXMLNodeName(layerProperty, "tileMap")) {
            if (!(layer->layerData = KON_LoadTileMap(layer, layerProperty)))
                return -1;
        } else if (KON_CompareXMLNodeName(layerProperty, "animArray")) {
            layer->keyFrameAnimationArray = KON_ParseKeyFrameAnimation(layerProperty, &layer->nbOfKeyFrameAnimations);
        }

        layerProperty = KON_GetXMLNodeSibling(layerProperty);
    }

    layer->playingAnimation = -1;
    layer->shown = true;
    KON_SystemMsg(message, MESSAGE_LOG, 0);

    return 0;
}

Map* KON_LoadMap(char* mapFilePath) {
    KON_XMLDocument* mapFile;
    KON_XMLNode* node;
    Map* loadedMap = NULL;
    unsigned int nbOfLayers, currentLayer;

    if (! mapFilePath) {
        KON_SystemMsg("(KON_LoadMap) No path provided!", MESSAGE_ERROR, 0);
        return NULL;
    }

    if (!(mapFile = KON_LoadXml(mapFilePath))) {
        KON_SystemMsg("(KON_LoadMap) Couldn't oppen map file :", MESSAGE_ERROR, 1, mapFilePath);
        return NULL;
    }

    if (!KON_CompareXMLNodeName(mapFile->rootNode, "map") || !(nbOfLayers = KON_GetXMLNodeCount(mapFile->rootNode, "layer"))) {
        KON_FreeXML(mapFile);
        KON_SystemMsg("(KON_LoadMap) Invalid map file!", MESSAGE_ERROR, 1, mapFilePath);
        return NULL;
    }

    if (!(loadedMap = (Map*)calloc(1, sizeof(Map)))) {
        KON_FreeXML(mapFile);
        KON_SystemMsg("(KON_LoadMap) No more memory!", MESSAGE_ERROR, 0);
        return NULL;
    }

    if (!(loadedMap->MapLayer = (MapLayer*)calloc(nbOfLayers, sizeof(MapLayer)))) {
        free(loadedMap);
        KON_FreeXML(mapFile);
        KON_SystemMsg("(KON_LoadMap) No more memory!", MESSAGE_ERROR, 0);
        return NULL;
    }

    loadedMap->nbOfLayers = nbOfLayers;
    loadedMap->MapFilePath = mapFilePath;

    /* Loads Each Layers*/
    node = KON_GetXMLNodeChild(mapFile->rootNode);
    for (currentLayer = 0; currentLayer < nbOfLayers; currentLayer++) {
        if (KON_LoadMapLayer(loadedMap->MapLayer + currentLayer, node)) {
            KON_FreeMap(&loadedMap);
            KON_SystemMsg("(KON_LoadMap) Invalid Layer in map:", MESSAGE_ERROR, 1, mapFilePath);
        }

        node = KON_GetXMLNodeSibling(node);
    }
    KON_FreeXML(mapFile);

    KON_SystemMsg("(KON_LoadMap) Loaded NEW Map :", MESSAGE_LOG, 1, mapFilePath);

    return loadedMap;
}

void KON_FreeMap(Map** map) {
    /* TODO Implement */
    free(*map);
    *map = NULL;
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

void KON_AddSpriteToLayer(Map* map, Sprite* sprite, unsigned int layerID, unsigned int priority, double x, double y) {
    if (layerID >= map->nbOfLayers) {
        KON_SystemMsg("(KON_AddSpriteToLayer) No such layer !", MESSAGE_ERROR, 0);
        return;
    }

    sprite->spritePosition.x = x;
    sprite->spritePosition.y = y;

    KON_AddToDisplayList(&map->MapLayer[layerID].displayList, sprite, priority);
}

void KON_PlayLayerAnimation(Map* map, unsigned int layerID, unsigned int animID) {
    if (!map || layerID >= map->nbOfLayers || animID >= map->MapLayer[layerID].nbOfKeyFrameAnimations) {
        KON_SystemMsg("(KON_PlayLayerAnimation) Invalid parameters!", MESSAGE_WARNING, 0);
        return;
    }

    map->MapLayer[layerID].playingAnimation = animID;
    map->MapLayer[layerID].keyFrameAnimationArray[animID].lastTimeCode = KON_GetMs();
    map->MapLayer[layerID].keyFrameAnimationArray[animID].currentKeyFrame = 0;
}

/*
    SUMMARY : Updates the layer position by its playing animation
    INPUT   : MapLayer* layer : The layer to update the animation for
*/
static void KON_UpdateLayerAnimation(MapLayer* layer) {
    if (layer->playingAnimation == -1)
        return;

    KON_UpdateAnimation(&layer->pos, &layer->keyFrameAnimationArray[layer->playingAnimation]);
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

        /* Update layer position */
        KON_UpdateLayerAnimation(currentLayer);

        /* Update all entities position */
        KON_UpdateLayerEntityPosition(currentLayer);

        KON_RenderLayer(currentLayer);
    }
}