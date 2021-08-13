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

void KON_MoveEntity(SceneHandle* scene, EntityInstance* entityInstancePointer, double X, double Y){
    MapLayer* currentLayer;
    Vector2i entityNewTile;
    unsigned int tileSize;

    if (scene && entityInstancePointer->commun->isSolid){ /* If we need to check colisions */
        /* we figure out where the entity is supposed to land */
        currentLayer = scene->WorldMap->MapLayer + entityInstancePointer->layerID;

        if (currentLayer->layerType == KON_LAYER_TILEMAP){
            tileSize = ((TileMap*)currentLayer->layerData)->TileSize;

            entityNewTile.x = (entityInstancePointer->pos.x + X) / tileSize;
            entityNewTile.y = (entityInstancePointer->pos.y + Y) / tileSize;
            
            /* X Collisions */
            if (!KON_IsMapTileSolid(scene, entityInstancePointer->layerID, entityNewTile.x, (int)entityInstancePointer->pos.y / tileSize, NULL)){
                entityInstancePointer->pos.x += X;
            } else {
                if (X > 0){
                    entityInstancePointer->pos.x = entityNewTile.x * tileSize - 1;
                } else {
                    entityInstancePointer->pos.x = (entityNewTile.x + 1) * tileSize;
                }
            }
            
            /* Y Collisions */
            if (!KON_IsMapTileSolid(scene, entityInstancePointer->layerID, entityInstancePointer->pos.x / tileSize, entityNewTile.y, NULL)){
                entityInstancePointer->pos.y += Y;
            } else {
                if (Y > 0){
                    entityInstancePointer->pos.y = entityNewTile.y * tileSize - 1;
                } else {
                    entityInstancePointer->pos.y = (entityNewTile.y + 1) * tileSize;
                }
            }
            return;
        }
    }

    entityInstancePointer->pos.x += X;
    entityInstancePointer->pos.y += Y;
}