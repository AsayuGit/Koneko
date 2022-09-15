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

#include "Entity.h"

#include "Animation.h"
#include "CommunFunctions.h"
#include "System.h"
#include "Graphics.h"
#include "Log.h"
#include "DisplayList.h"

#include <stdlib.h>

/* Loads an Entity in memory */
EntityInstance* KON_LoadEntity(EntityDescriptor* entityToLoad){
    EntityInstance* newEntityInstance = NULL;

    /* Init */
    if (!(newEntityInstance = (EntityInstance*)calloc(1, sizeof(EntityInstance)))) {
        KON_SystemMsg("(KON_LoadEntity) Not enough memory !", MESSAGE_ERROR, 0);
        return NULL;
    }
    newEntityInstance->descriptor = entityToLoad;

    /* Load */
    newEntityInstance->entitySprite = KON_LoadSpriteFromXml(entityToLoad->spriteXmlPath);

    /* Copy properties over */
    newEntityInstance->properties = entityToLoad->properties;

    return newEntityInstance;
}

void KON_FreeEntity(EntityInstance* entityInstanceToFree){
    if (entityInstanceToFree->descriptor->OnExit)
        entityInstanceToFree->descriptor->OnExit(entityInstanceToFree);

    KON_FreeSprite(&entityInstanceToFree->entitySprite);

    KON_FreeLinkedList(entityInstanceToFree->collision.collisionEvents);
    KON_FreeLinkedList(entityInstanceToFree->collision.collisionEvents + 1);

    free(entityInstanceToFree);
}

/* KON_UpdateEntity() */
void KON_UpdateEntityPosition(EntityInstance* entity) { /* Display "A" Character on screen  */
    entity->lastPos = entity->pos;
    entity->entitySprite->spritePosition = entity->pos = KON_GetVectAddition(&entity->pos, &entity->mov);
    entity->mov = KON_InitVector2d(0, 0);
}

void KON_PlayEntityAnimation(EntityInstance* entity, unsigned int animationID, bool reset, bool loop){
    KON_PlaySpriteAnimation(entity->entitySprite, animationID, reset, loop);
}

/*
    SUMMARY : Calls the relevant entity collision callbacks.
    INPUT   : SceneHandle* scene     : The scene in which the collisions happened.
    INPUT   : MapLayer* layer        : The layer in which the collisions happened.
    INPUT   : EntityInstance* entity : The entity to process the callbacks of.
*/
static void KON_ProcessEntityCollisionsCall(SceneHandle* scene, MapLayer* layer, EntityInstance* entity){
    bool frameSelect = entity->collision.collisionFrameSelect;
    LinkedList* nowColidingEntities = entity->collision.collisionEvents[frameSelect];
    LinkedList* wereColidingEntities = entity->collision.collisionEvents[frameSelect ^ 1];
    EntityDescriptor* call = entity->descriptor;

    while (nowColidingEntities){
        if (!((CollisionEvent*)nowColidingEntities->data)->entityGenerateCollisionEvents) {
            nowColidingEntities = nowColidingEntities->next;
            continue;
        }

        if (KON_SearchDataInLinkedList(wereColidingEntities, nowColidingEntities->data)){
            /* Ongoing collision */

            if (call->OnCollisionStay) {
                /*KON_SystemMsg("(KON_ProcessEntityCollisionsCalls) Stay", MESSAGE_WARNING, 1, entity->descriptor->entityName);*/
                call->OnCollisionStay(scene, layer, entity, (CollisionEvent*)nowColidingEntities->data);
            }
        } else {
            /* New collision */

            if (call->OnCollisionStart) {
                /*KON_SystemMsg("(KON_ProcessEntityCollisionsCalls) Start", MESSAGE_WARNING, 1, entity->descriptor->entityName);*/
                call->OnCollisionStart(scene, layer, entity, (CollisionEvent*)nowColidingEntities->data);
            }
        }

        nowColidingEntities = nowColidingEntities->next;
    }
    nowColidingEntities = entity->collision.collisionEvents[frameSelect];
    while (wereColidingEntities){
        if (!((CollisionEvent*)wereColidingEntities->data)->entityGenerateCollisionEvents) {
            wereColidingEntities = wereColidingEntities->next;
            continue;
        }

        if (!KON_SearchDataInLinkedList(nowColidingEntities, wereColidingEntities->data)){
            /* Outgoing collision */

            if (call->OnCollisionStop) {
                /*KON_SystemMsg("(KON_ProcessEntityCollisionsCalls) Stop", MESSAGE_WARNING, 1, entity->descriptor->entityName);*/
                call->OnCollisionStop(scene, layer, entity, (CollisionEvent*)wereColidingEntities->data);
            }
        }
        wereColidingEntities = wereColidingEntities->next;
    }
}

void KON_ProcessEntityCollisionsCalls(SceneHandle* scene){
    EntityInstance *entityInstance;
    LinkedList* entityInstanceList;
    MapLayer* layer;
    unsigned int i;
    
    /* For each map plane */
    for (i = 0; i < scene->WorldMap->nbOfLayers; i++) {
        layer = scene->WorldMap->MapLayer + i;
        entityInstanceList = layer->entityInstanceList;
        while (entityInstanceList){
            entityInstance = ((EntityInstance*)entityInstanceList->data);

            if (entityInstance->collision.generateCollisionEvents)
                KON_ProcessEntityCollisionsCall(scene, layer, entityInstance);

            entityInstanceList = entityInstanceList->next;
        }
    }
}

void KON_BoundEntityInstanceToRect(EntityInstance* entity, KON_Rect* rect){
    unsigned char result;
    Vector2d vect = KON_GetVectAddition(&entity->pos, &entity->mov);

    result = KON_BoundVect2dToRect(&vect, rect);
    if (result & 1){
        entity->mov.x = vect.x - entity->pos.x;
    }
    if (result & 2){
        entity->mov.y = vect.y - entity->pos.y;
    }
}

EntityInstance* KON_SpawnEntity(SceneHandle* scene, EntityDescriptor* spawnedEntity, unsigned int layerID, unsigned int priority, double x, double y) {
    EntityInstance* newInstance = NULL;
    LinkedList* nodePointer = NULL;
    MapLayer* mapLayer;

    if (layerID >= scene->WorldMap->nbOfLayers) {
        KON_SystemMsg("(KON_SpawnEntity) Invalid LayerID!", MESSAGE_WARNING, 0);
        return NULL;
    }

    if (!(newInstance = KON_LoadEntity(spawnedEntity))) {
        KON_SystemMsg("(KON_SpawnEntity) Couldn't load Entity!", MESSAGE_WARNING, 0);
        return NULL;
    }

    /* Load the new entity in memory */
    newInstance->pos.x = x;
    newInstance->pos.y = y;
    newInstance->collision.generateCollisionEvents = true;

    /* The MapLayer to spawn the new entity in */
    mapLayer = scene->WorldMap->MapLayer + layerID;

    if (newInstance->descriptor->OnSetup)
        newInstance->descriptor->OnSetup(scene, mapLayer, newInstance);

    nodePointer = KON_AppendToLinkedList(&mapLayer->entityInstanceList, newInstance, sizeof(EntityInstance));
    free(newInstance);

    /* Add the entity's sprite to its mapLayer's DisplayList */
    KON_AddToDisplayList(&mapLayer->displayList, ((EntityInstance*)nodePointer->data)->entitySprite, priority);

    return ((EntityInstance*)nodePointer->data);
}

/* FIXME: Implement */
void KON_KillEntityInstance(SceneHandle* scene, EntityInstance* entityInstanceToKill) {
    KON_SystemMsg("(KON_KillEntityInstance) Not Implemented !", MESSAGE_WARNING, 0);
}