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
/* TODO: This curently mallocs an instance twice due to how KON_appendToList works
            and could be tweaked for dynamically allowed entities */
EntityInstance* KON_SpawnEntity(KONDevice* KDevice, SceneHandle* scene, EntityDescriptor* SpawnedEntity, unsigned int layerID, unsigned int X, unsigned int Y){
    Entity* loadedEntity = NULL;
    EntityInstance* newInstance = NULL;
    LinkedList* nodePointer = NULL;

    /* Create a new instance to spawn in */
    newInstance = (EntityInstance*)calloc(1, sizeof(EntityInstance));

    /* We check if the entity is already loaded in memory */
    nodePointer = scene->entityList;
    while (nodePointer){
        if (((Entity*)nodePointer->data)->descriptor == SpawnedEntity){
            loadedEntity = ((Entity*)nodePointer->data);
            break;
        }
        nodePointer = nodePointer->next;
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
    newInstance->layerID = layerID;
    newInstance->isVisible = true;
    newInstance->collision.generateCollisionEvents = true;

    if (newInstance->commun->descriptor->OnSetup)
        newInstance->commun->descriptor->OnSetup(KDevice, scene, newInstance);

    nodePointer = KON_appendToList(&scene->entityInstanceList, newInstance, sizeof(EntityInstance));
    free(newInstance);
    
    return ((EntityInstance*)nodePointer->data);
}

void KON_KillEntity(SceneHandle* scene, Entity* entityToKill){
    LinkedList** entityList = &scene->entityList;
    
    while (*entityList){

        if ((Entity*)(*entityList)->data == entityToKill){
            KON_FreeEntity(entityToKill);
            KON_DeleteListNode(entityList);
        }

        if (!*entityList)
            break;
        entityList = &(*entityList)->next;
    }
}

/* TODO :3 */
void KON_KillEntityInstance(SceneHandle* scene, EntityInstance* entityInstanceToKill){
    LinkedList** entityInstanceList = &scene->entityInstanceList;
    Entity* parentEntity = NULL;
    bool instanceKilled = false;

    while (*entityInstanceList){
        if ((EntityInstance*)(*entityInstanceList)->data == entityInstanceToKill){
            if (instanceKilled){
                return;
            } else {
                parentEntity = ((EntityInstance*)(*entityInstanceList)->data)->commun;
                /* Free entity instance */
                KON_FreeEntityInstance(entityInstanceToKill);
                KON_DeleteListNode(entityInstanceList); /* Don't forget to clean the instance's context :3 */
                instanceKilled = true;
            }
        }
        if (!*entityInstanceList)
            break;
        entityInstanceList = &(*entityInstanceList)->next;
    }
    /* Free parent entity if no more instances are found */
    if (parentEntity)
        KON_KillEntity(scene, parentEntity);
}

int KON_StartScene(KONDevice* KDevice, SceneDescriptor* scenePointer){
    SceneHandle* scene = NULL;
    EntityInstance* entityInstancePointer = NULL;
    LinkedList* nodePointer = NULL;
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
    if (scenePointer->OnSetup)
        scenePointer->OnSetup(KDevice, scene);


    /* Main Loop */
    while (true){

        /* Events Loop */
        while(SDL_PollEvent(&KDevice->IDevice->event)){
            KON_SystemEvents(KDevice->DDevice, KDevice->IDevice); /* Engine events */
            if (scenePointer->OnEvent)
                scenePointer->OnEvent(KDevice, scene);

            nodePointer = scene->entityInstanceList;
            while (nodePointer){
                entityInstancePointer = ((EntityInstance*)nodePointer->data);
                if (entityInstancePointer->commun->descriptor->OnEvent)
                    entityInstancePointer->commun->descriptor->OnEvent(KDevice, scene, entityInstancePointer);

                nodePointer = nodePointer->next;
            }
        }

        if (scenePointer->OnFrame)
            scenePointer->OnFrame(KDevice, scene);

        /* Entity OnFrame Logic */
        nodePointer = scene->entityInstanceList;
        while (nodePointer){
            entityInstancePointer = ((EntityInstance*)nodePointer->data);
            if (entityInstancePointer->commun->descriptor->OnFrame)
                entityInstancePointer->commun->descriptor->OnFrame(KDevice, scene, entityInstancePointer);

            nodePointer = nodePointer->next;
        }

        KON_EntityCollisions(KDevice, scene);

        /* Music loop deamon */
        KON_MusicDaemon();

        /* Clear the screen before rendering a new frame */
        SDL_RenderClear(KDevice->DDevice->Renderer);

        /* KON_Draw */
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

            /* KON_Draw all entities in the scene */
            nodePointer = scene->entityInstanceList;
            while (nodePointer){ /* Processing all entity instances */
                entityInstancePointer = ((EntityInstance*)nodePointer->data);
                if (entityInstancePointer->layerID == i) {
                    KON_DrawEntity(KDevice->DDevice, entityInstancePointer);
                    if (entityInstancePointer->commun->descriptor->OnDraw)
                        entityInstancePointer->commun->descriptor->OnDraw(KDevice, scene, entityInstancePointer);
                }
                nodePointer = nodePointer->next;
            }

            /* Custom On_Draw() event */
            if (scenePointer->OnDraw)
                scenePointer->OnDraw(KDevice, scene);

        }

        KON_FinishFrame(KDevice->DDevice);                                   /* Update the main window */

        SDL_Delay(0); /* TEMPORARY Debug delay */
    }

    if (scenePointer->OnExit)
        scenePointer->OnExit(KDevice, scene);

    return returnValue;
}