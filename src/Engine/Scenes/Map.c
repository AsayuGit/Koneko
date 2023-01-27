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

#include "Map.h"
#include "Koneko.h"
#include "TileMap.h"
#include "KON_Raycast.h"
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

static int KON_LoadMapLayer(MapLayer* layer, KON_XMLNode* layerNode) {
    const char* layerType = KON_GetXMLAttribute(layerNode, "type");

    if (strcmp(layerType, "BitMap") == 0) {
        return KON_LoadBitMapLayer(layer, layerNode);
    } else if (strcmp(layerType, "TileMap") == 0) {
        return KON_LoadTileMapLayer(layer, layerNode);
    } else if (strcmp(layerType, "Raycast") == 0) {
        return KON_LoadRaycastLayer(layer, layerNode);
    } else if (strcmp(layerType, "Empty") == 0) {
        layer->layerRenderer = RENDER_NONE;
        KON_SystemMsg("(KON_LoadMapLayer) Loaded NEW Empty Layer", MESSAGE_LOG, 0);
        layer->playingAnimation = -1;
        layer->shown = true;
        return 0;
    }
    
    return -1;
}

void KON_LoadLayerSurface(MapLayer* layer, KON_XMLNode* layerNode) {
    uint32_t colorKey;
    const char* colorKeyString, *surfacePath = KON_GetXMLAttribute(layerNode, "surfacePath");

    if ((colorKeyString = KON_GetXMLAttribute(layerNode, "colorKey"))) {
        sscanf(colorKeyString, "0x%x", &colorKey);
        layer->texture.gpuSide = KON_LoadSurface(surfacePath, colorKey, SURFACE_KEYED);
    } else {
        layer->texture.gpuSide = KON_LoadSurface(surfacePath, 0, SURFACE_ALPHA);
    }
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

void KON_RemoveSpriteFromLayer(Map* map, Sprite* sprite, unsigned int layerID) {
    KON_RemoveFromDisplayList(&map->MapLayer[layerID].displayList, sprite);
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

void KON_DrawMap(SceneHandle* scene, functPtrOnDisplay OnDisplay) {
    MapLayer* currentLayer;
    unsigned int i;
    
    /* Clear the screen before rendering a new frame */
    KON_ClearScreen();

    /* Draw each layer */
    for (i = 0; i < scene->WorldMap->nbOfLayers; i++) {
        currentLayer = scene->WorldMap->MapLayer + i;

        /* Update layer position */
        KON_UpdateLayerAnimation(currentLayer);

        /* Update all entities position */
        KON_UpdateLayerEntityPosition(currentLayer);

        KON_RenderLayer(currentLayer);

        if (OnDisplay)
            OnDisplay(scene, i);
    }
}
