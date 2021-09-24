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

#include "Scene.h"
#include "System.h"
#include "Load.h"
#include "Jukebox.h"
#include "Graphics.h"

#include "CommunFunctions.h"

/* Spawns an entity in the specified scene, loading it if necessary */
EntityInstance* KON_SpawnEntity(KONDevice* KDevice, SceneHandle* scene, EntityDescriptor* SpawnedEntity, unsigned int layerID, unsigned int X, unsigned int Y){
    Entity* loadedEntity = NULL;
    EntityInstance* newInstance = NULL;
    Node* nodePointer = NULL;

    /* Create a new instance to spawn in */
    newInstance = (EntityInstance*)calloc(1, sizeof(EntityInstance));

    /* We check if the entity is already loaded in memory */
    nodePointer = scene->entityList;
    while (nodePointer){
        if (((Entity*)nodePointer->data)->descriptor == SpawnedEntity){
            loadedEntity = ((Entity*)nodePointer->data);
            break;
        }
        nodePointer = (Node*)nodePointer->next;
    }
    /* Load the entity in memory if not found */
    if (!loadedEntity){
        loadedEntity = KON_LoadEntity(KDevice->DDevice, SpawnedEntity);
        /*printf("Loading entity %s\n", SpawnedEntity->EntityDesctiptorPath);*/
    }

    /* Load the new entity in memory */
    newInstance->commun = ((Entity*)KON_appendToList(&scene->entityList, loadedEntity, sizeof(Entity))->data);
    newInstance->pos.x = X;
    newInstance->pos.y = Y;
    newInstance->commun->descriptor->OnSetup(KDevice, scene, newInstance);
    newInstance->layerID = layerID;

    nodePointer = KON_appendToList(&scene->entityInstanceList, newInstance, sizeof(EntityInstance));
    free(newInstance);
    
    return ((EntityInstance*)nodePointer->data);
}

int KON_StartScene(KONDevice* KDevice, SceneDescriptor* scenePointer){
    SceneHandle* scene = NULL;
    EntityInstance* entityInstancePointer = NULL;
    Node* nodePointer = NULL;
    MapLayer* currentLayer = NULL;
    int returnValue = 0;
    int renderer = 1; /* TEMP */
    int i;

    scene = (SceneHandle*)calloc(1, sizeof(SceneHandle));
    scene->WorldMap = KON_LoadMap(KDevice->DDevice, scenePointer->WorldMapPath);
    if (!scene->WorldMap){
        printf("Error Loading Scene Data !\n");
        exit(-1);
    }
    scenePointer->OnSetup(KDevice, scene);


    /* Main Loop */
    while (true){

        /* Events Loop */
        while(SDL_PollEvent(&KDevice->IDevice->event)){
            SystemEvents(KDevice->DDevice, KDevice->IDevice); /* Engine events */
            scenePointer->OnEvent(KDevice, scene);

            nodePointer = scene->entityInstanceList;
            while (nodePointer){
                entityInstancePointer = ((EntityInstance*)nodePointer->data);
                entityInstancePointer->commun->descriptor->OnEvent(KDevice, scene, entityInstancePointer);

                nodePointer = (Node*)nodePointer->next;
            }
        }

        scenePointer->OnFrame(KDevice, scene);

        /* Entity OnFrame Logic */
        nodePointer = scene->entityInstanceList;
        while (nodePointer){
            entityInstancePointer = ((EntityInstance*)nodePointer->data);
            entityInstancePointer->commun->descriptor->OnFrame(KDevice, scene, entityInstancePointer);

            nodePointer = (Node*)nodePointer->next;
        }

        KON_EntityColisions(KDevice, scene);

        /* Music loop deamon */
        KON_MusicDaemon();

        /* Clear the screen before rendering a new frame */
        SDL_RenderClear(KDevice->DDevice->Renderer);

        /* Draw */
        for (i = scene->WorldMap->nbOfLayers - 1; i >= 0; i--){
            switch (renderer) {
                case 1: /* 2D */
                    currentLayer = scene->WorldMap->MapLayer + i;
                    switch (currentLayer->layerType)
                    {
                        case KON_LAYER_BITMAP:
                            KON_DrawBitMap(KDevice->DDevice, currentLayer);
                            break;

                        case KON_LAYER_TILEMAP:
                            KON_DrawTileMap(KDevice->DDevice, currentLayer);
                            break;
                        
                        default:
                            break;
                    }    
                    break;

                case 2: /* Raycast */
                    /* code */
                    break;
                    
                default:
                    break;
            }

            /* Draw all entities in the scene */
            nodePointer = scene->entityInstanceList;
            while (nodePointer){ /* Processing all entity instances */
                entityInstancePointer = ((EntityInstance*)nodePointer->data);
                if (entityInstancePointer->layerID == i)
                    KON_DrawEntity(KDevice->DDevice, entityInstancePointer);
                nodePointer = (Node*)nodePointer->next;
            }
        }

        FinishFrame(KDevice->DDevice);                                   /* Update the main window */

        SDL_Delay(0); /* TEMPORARY Debug delay */
    }

    scenePointer->OnExit(KDevice, scene);

    return returnValue;
}