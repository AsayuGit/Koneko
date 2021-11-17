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
            /* FIXME: We should probably crash and generate an error if the value isn't true nor false */
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
        entityFile = KON_LoadXml(entityToLoad->EntityDesctiptorPath); /* Load the xml file in memory */
        character = xmlDocGetRootElement(entityFile); /* root node */

        /* Logic */
        if ((SurfacePath = (char*)xmlGetProp(character, (xmlChar*)"texture"))){
            if ((Buffer = (char*)xmlGetProp(character, (xmlChar*)"colorKey"))){
                sscanf(Buffer, "%x", &ColorKey);
                newEntity->EntityTexture = KON_LoadSurface(SurfacePath, DDevice, ColorKey, SURFACE_KEYED);
            } else {
                newEntity->EntityTexture = KON_LoadSurface(SurfacePath, DDevice, 0x0, SURFACE_OPAQUE);
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

void KON_FreeEntityInstance(EntityInstance* entityInstanceToFree){
    if (entityInstanceToFree->commun->descriptor->OnExit)
        entityInstanceToFree->commun->descriptor->OnExit(entityInstanceToFree);

    KON_FreeLinkedList(entityInstanceToFree->collision.collisionEvents);
    KON_FreeLinkedList(entityInstanceToFree->collision.collisionEvents + 1);

    free(entityInstanceToFree);
}

void KON_FreeEntity(Entity* entityToFree){
    KON_FreeSurface(entityToFree->EntityTexture);
    KON_FreeAnimation(entityToFree->entityAnimations);
    free(entityToFree->entityName);
    free(entityToFree);
}

/* potential caching possible (entity->commun->entityAnimations)*/
void KON_DrawEntity(DisplayDevice* DDevice, EntityInstance* entity){ /* Display "A" Character on screen  */
    SDL_Rect SpriteWindow, SpriteLayer;
    Animation* entityPlayingAnim;

    if (entity->isVisible){
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

        KON_ScaledDrawEx(DDevice, entity->commun->EntityTexture, &SpriteWindow, &SpriteLayer, entity->Flip);
        
        if (SDL_GetTicks() > entity->LastFrame + entityPlayingAnim->Framerate){
            entity->LastFrame = SDL_GetTicks();
            entity->CurrentFrame++;
        }
    }
}

void KON_EntityPlayAnimation(EntityInstance* entity, unsigned int AnimationID, bool reset, bool loop){
    if (reset || AnimationID != entity->PlayingAnimation){
        entity->PlayingAnimation = AnimationID;
        entity->CurrentFrame = 0;
        entity->alimationLoop = loop;
    }
}

bool KON_FindInEntityInstanceList(LinkedList* list, void* data){
    while (list){
        if (*(EntityInstance**)list->data == *(EntityInstance**)data)
            return true;
        list = list->next;
    }
    return false;
}

void KON_ProcessEntityCollisionsCalls(KONDevice* KDevice, SceneHandle* scene, EntityInstance* entity){
    bool frameSelect = entity->collision.collisionFrameSelect;
    LinkedList* nowColidingEntities = entity->collision.collisionEvents[frameSelect];
    LinkedList* wereColidingEntities = entity->collision.collisionEvents[frameSelect ^ 1];
    EntityDescriptor* call = entity->commun->descriptor;

    while (nowColidingEntities){
        if (KON_FindInEntityInstanceList(wereColidingEntities, nowColidingEntities->data)){
            /* Ongoing collision */
            if (call->OnCollisionStay)
                call->OnCollisionStay(KDevice, scene, entity, (CollisionEvent*)nowColidingEntities->data);
        } else {
            /* New collision */
            if (call->OnCollisionStart)
                call->OnCollisionStart(KDevice, scene, entity, (CollisionEvent*)nowColidingEntities->data);
        }
        nowColidingEntities = nowColidingEntities->next;
    }
    nowColidingEntities = entity->collision.collisionEvents[frameSelect];
    while (wereColidingEntities){
        if (!KON_FindInEntityInstanceList(nowColidingEntities, wereColidingEntities->data)){
            /* Outgoing collision */
            if (call->OnCollisionStop)
                call->OnCollisionStop(KDevice, scene, entity, (CollisionEvent*)wereColidingEntities->data);
        }
        wereColidingEntities = wereColidingEntities->next;
    }
}

void KON_BoundEntityInstanceToRect(EntityInstance* entity, SDL_Rect* rect){
    unsigned char result;
    Vector2d vect = KON_GetVectAddition(entity->pos, entity->mov);

    result = KON_BoundVect2dToRect(&vect, rect);
    if (result & 1){
        entity->mov.x = vect.x - entity->pos.x;
    }
    if (result & 2){
        entity->mov.y = vect.y - entity->pos.y;
    }
}