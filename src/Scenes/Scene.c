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

#include "Koneko.h"
#include "Scene.h"
#include "Graphics.h"
#include "TileMap.h"
#include "Log.h"
#include "DisplayList.h"
#include "KON_Music.h"

#include "CommunFunctions.h"

#include <stdlib.h>
#include <stdarg.h>

#define KON_EntityOnX(scene, function) {                                                                 \
    MapLayer* currentLayer;                                                                              \
    LinkedList* entityInstanceList;                                                                      \
    EntityInstance* entityInstancePointer;                                                               \
    unsigned int i;                                                                                      \
                                                                                                         \
    /* For each layer */                                                                                 \
    for (i = 0; i < scene->WorldMap->nbOfLayers; i++){                                                   \
        currentLayer = scene->WorldMap->MapLayer + i;                                                    \
                                                                                                         \
        /* For each entity instance */                                                                   \
        entityInstanceList = currentLayer->entityInstanceList;                                           \
        while (entityInstanceList) {                                                                     \
                                                                                                         \
            entityInstancePointer = ((EntityInstance*)entityInstanceList->data);                         \
            if (entityInstancePointer->enabled && entityInstancePointer->descriptor->function)           \
                entityInstancePointer->descriptor->function(scene, currentLayer, entityInstancePointer); \
                                                                                                         \
            entityInstanceList = entityInstanceList->next;                                               \
        }                                                                                                \
    }                                                                                                    \
}                                                                                                        \

static void KON_MapEvents(SceneHandle* scene) { KON_EntityOnX(scene, OnEvent); }
static void KON_MapFrame(SceneHandle* scene) { KON_EntityOnX(scene, OnFrame); }

static bool sceneAlive;

int KON_StartScene(SceneDescriptor* scenePointer, ...) {
    SceneHandle* scene;

    scene = (SceneHandle*)calloc(1, sizeof(SceneHandle));
    scene->WorldMap = KON_LoadMap(scenePointer->WorldMapPath);
    if (!scene->WorldMap)
        KON_SystemMsg("(KON_StartScene) Error Loading Scene Data !", MESSAGE_ERROR, 0);
    if (scenePointer->OnSetup) {
        va_list args;
        va_start(args, scenePointer);
        scenePointer->OnSetup(scene, args);
        va_end(args);
    }
    /* Main Loop */
    sceneAlive = true;
    while (sceneAlive) {
        /* Events Loop */
        KON_PumpEvent();
        while (KON_PollEvent()){
            KON_SystemEvents(); /* Engine events */

            if (scenePointer->OnEvent)
                scenePointer->OnEvent(scene);

            KON_MapEvents(scene);
        }
        
        if (scenePointer->OnFrame)
            scenePointer->OnFrame(scene);

        /* Entity OnFrame Logic */
        KON_MapFrame(scene);

        /* Process entitiy collisions */
        /* TODO: Fix -> KON_ProcessEntityCollisions(scene);*/

        /* Music loop deamon */
        KON_MusicDaemon();

        /* Draws the loaded map and its content on scren */
        KON_DrawMap(scene->WorldMap);

        if (scenePointer->OnDisplay)
            scenePointer->OnDisplay(scene);

        KON_FinishFrame(); /* Update the main window */
    }

    if (scenePointer->OnExit)
        scenePointer->OnExit(scene);

    return 0;
}

void KON_EndScene(SceneHandle* scene) {
    sceneAlive = false;
}