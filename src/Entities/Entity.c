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

#include "Entity.h"
#include "Load.h"
#include "Animation.h"
#include "CommunFunctions.h"
#include "System.h"
#include "Graphics.h"

void KON_ParseEntityFlags(Entity* entity, xmlNode* flags){
    flags = flags->children;
    while (flags){
        if (strcmp((char*)flags->name, "isSolid") == 0) {
            entity->isSolid = (strcmp((char*)xmlGetProp(flags, (xmlChar*)"value"), "true") == 0);
        }
        flags = flags->next;
    }
}

/* Loads an Entity in memory */
Entity* KON_LoadEntity(DisplayDevice* DDevice, EntityDescriptor* entityToLoad){
    Entity* newEntity = NULL;
    xmlDoc* entityFile;
    xmlNode *character, *property;
    Uint32 ColorKey;
    char* SurfacePath, *Buffer;

    /* Init */
    newEntity = (Entity*)calloc(1, sizeof(Entity));

    newEntity->descriptor = entityToLoad;

    if (entityToLoad->EntityDesctiptorPath){
        entityFile = loadXml(entityToLoad->EntityDesctiptorPath); /* Load the xml file in memory */
        character = xmlDocGetRootElement(entityFile); /* root node */

        /* Logic */
        if ((SurfacePath = (char*)xmlGetProp(character, (xmlChar*)"texture"))){
            if ((Buffer = (char*)xmlGetProp(character, (xmlChar*)"colorKey"))){
                sscanf(Buffer, "%x", &ColorKey);
                newEntity->EntityTexture = LoadSurface(SurfacePath, DDevice, ColorKey, SURFACE_KEYED);
            } else {
                newEntity->EntityTexture = LoadSurface(SurfacePath, DDevice, 0x0, SURFACE_OPAQUE);
            }
        }

        /* Parsing */
        property = character->children;
        while (property){
            if (strcmp((char*)property->name, "name") == 0){
                astrcpy(&newEntity->entityName ,(char*)xmlNodeGetContent(property));
            } else if (strcmp((char*)property->name, "animArray") == 0){
                newEntity->entityAnimations = KON_ParseAnimation(property);
            } else if (strcmp((char*)property->name, "flags") == 0){
                KON_ParseEntityFlags(newEntity, property);
            }
            property = property->next;
        }

        if (!newEntity->entityName){
            newEntity->entityName = "Unnamed Entity";
        }
    }

    xmlFreeDoc(entityFile);

    return newEntity;
}

/* potential caching possible (entity->commun->entityAnimations)*/
void KON_DrawEntity(DisplayDevice* DDevice, EntityInstance* entity){ /* Display "A" Character on screen  */
    SDL_Rect SpriteWindow, SpriteLayer;
    Animation* entityPlayingAnim;

    entityPlayingAnim = entity->commun->entityAnimations + entity->PlayingAnimation;

    /* On veille a ne pas dépacer le nombre de frames de l'animation */
    if (entity->CurrentFrame >= entityPlayingAnim->NbOfFrames){
        if (entity->alimationLoop){
            entity->CurrentFrame = 0;
        } else {
            entity->CurrentFrame = entityPlayingAnim->NbOfFrames - 1;
        }
    }
    
    /* On déplace la fenêtre dans la spritesheet en fonction du numéro de la frame */
    SpriteWindow = entityPlayingAnim->SrcRect;
    SpriteWindow.x = entityPlayingAnim->SrcRect.x + entity->CurrentFrame * entityPlayingAnim->SrcRect.w;

    SpriteLayer = entityPlayingAnim->DstRect;

    if (entity->Flip)
        SpriteLayer.x = -(SpriteLayer.x + entityPlayingAnim->DstRect.w);
    
    SpriteLayer.x += entity->pos.x - DDevice->Camera.x;
    SpriteLayer.y = entityPlayingAnim->DstRect.y + entity->pos.y - DDevice->Camera.y;

    entity->boundingBox = SpriteLayer;

    ScaledDrawEx(DDevice, entity->commun->EntityTexture, &SpriteWindow, &SpriteLayer, entity->Flip);
    
    if (SDL_GetTicks() > entity->LastFrame + entityPlayingAnim->Framerate){
        entity->LastFrame = SDL_GetTicks();
        entity->CurrentFrame++;
    }
}

void KON_EntityPlayAnimation(EntityInstance* entity, unsigned int AnimationID, bool reset, bool loop){
    if (reset || AnimationID != entity->PlayingAnimation){
        entity->PlayingAnimation = AnimationID;
        entity->CurrentFrame = 0;
        entity->alimationLoop = loop;
    }
}

void KON_EntitySceneCollisionCheck(SceneHandle* scene, EntityInstance* entityInstancePointer){
    MapLayer* currentLayer;
    Vector2i entityNewTile;
    unsigned int tileSize;

    if (scene && entityInstancePointer->commun->isSolid){ /* If we need to check colisions */
        /* we figure out where the entity is supposed to land */
        currentLayer = scene->WorldMap->MapLayer + entityInstancePointer->layerID;

        if (currentLayer->layerType == KON_LAYER_TILEMAP){
            tileSize = ((TileMap*)currentLayer->layerData)->TileSize;

            entityNewTile.x = (entityInstancePointer->pos.x + entityInstancePointer->mov.x) / tileSize;
            entityNewTile.y = (entityInstancePointer->pos.y + entityInstancePointer->mov.y) / tileSize;
            
            /* X Collisions */
            if (KON_IsMapTileSolid(scene, entityInstancePointer->layerID, entityNewTile.x, (int)entityInstancePointer->pos.y / tileSize, NULL)){
                if (entityInstancePointer->mov.x > 0){
                    entityInstancePointer->mov.x = (entityNewTile.x * tileSize - 1) - entityInstancePointer->pos.x;
                } else {
                    entityInstancePointer->mov.x = ((entityNewTile.x + 1) * tileSize) - entityInstancePointer->pos.x;
                }
                /*printf("DEBUG: EntitySceneCollision on X axis\n");*/
            }
            
            /* Y Collisions */
            if (KON_IsMapTileSolid(scene, entityInstancePointer->layerID, entityInstancePointer->pos.x / tileSize, entityNewTile.y, NULL)){
                if (entityInstancePointer->mov.y > 0){
                    entityInstancePointer->mov.y = (entityNewTile.y * tileSize - 1) - entityInstancePointer->pos.y;
                } else {
                    entityInstancePointer->mov.y = ((entityNewTile.y + 1) * tileSize) - entityInstancePointer->pos.y;
                }
                /*printf("DEBUG: EntitySceneCollision on Y axis\n");*/
            }
            return;
        }
    }
}

void KON_EntityEntityCollisionCheck(KONDevice* KDevice, SceneHandle* scene, EntityInstance* entityA, EntityInstance* entityB) {
    SDL_Rect collisionResult;

    if (SDL_IntersectRect(&entityA->boundingBox, &entityB->boundingBox, &collisionResult)) {
        entityA->commun->descriptor->OnColision(KDevice, scene, entityA, entityB);
    }
}

void KON_EntityColisions(KONDevice* KDevice, SceneHandle* scene) {
    EntityInstance *entityInstancePointer;
    Node* nodePointer, *nodePointerB;

    /* Entity / Scene Collisions */
    nodePointer = scene->entityInstanceList;
    while (nodePointer){
        KON_EntitySceneCollisionCheck(scene, (EntityInstance*)nodePointer->data);
        nodePointer = (Node*)nodePointer->next;
    }

    /* Entity / Entity Collisions */
    nodePointer = nodePointerB = scene->entityInstanceList;
    while (nodePointer){
        entityInstancePointer = ((EntityInstance*)nodePointer->data);

        while (nodePointerB){
            if (nodePointer != nodePointerB){
                KON_EntityEntityCollisionCheck(KDevice, scene, entityInstancePointer, (EntityInstance*)nodePointerB->data);
            }

            nodePointerB = (Node*)nodePointerB->next;
        }

        entityInstancePointer->pos.x += entityInstancePointer->mov.x;
        entityInstancePointer->pos.y += entityInstancePointer->mov.y;

        entityInstancePointer->mov = InitVector2d(0.0, 0.0);

        nodePointer = (Node*)nodePointer->next;
    }

    /* TODO: Reset entity->mov when done */
}