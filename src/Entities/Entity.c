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
    KON_LoadSpriteFromXml(&newEntityInstance->entitySprite, entityToLoad->spriteXmlPath);

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

/* potential caching possible (entity->commun->entityAnimations)*/
void KON_DrawEntity(EntityInstance* entity) { /* Display "A" Character on screen  */
    entity->entitySprite.spritePosition = entity->pos;
    KON_DrawSprite(&entity->entitySprite);
}

void KON_PlayEntityAnimation(EntityInstance* entity, unsigned int animationID, bool reset, bool loop){
    KON_PlaySpriteAnimation(&entity->entitySprite, animationID, reset, loop);
}

bool KON_FindInEntityInstanceList(LinkedList* list, void* data){
    while (list){
        if (*(EntityInstance**)list->data == *(EntityInstance**)data)
            return true;
        list = list->next;
    }
    return false;
}

void KON_ProcessEntityCollisionsCalls(SceneHandle* scene, EntityInstance* entity){
    bool frameSelect = entity->collision.collisionFrameSelect;
    LinkedList* nowColidingEntities = entity->collision.collisionEvents[frameSelect];
    LinkedList* wereColidingEntities = entity->collision.collisionEvents[frameSelect ^ 1];
    EntityDescriptor* call = entity->descriptor;

    while (nowColidingEntities){
        if (KON_FindInEntityInstanceList(wereColidingEntities, nowColidingEntities->data)){
            /* Ongoing collision */
            if (call->OnCollisionStay)
                call->OnCollisionStay(scene, entity, (CollisionEvent*)nowColidingEntities->data);
        } else {
            /* New collision */
            if (call->OnCollisionStart)
                call->OnCollisionStart(scene, entity, (CollisionEvent*)nowColidingEntities->data);
        }
        nowColidingEntities = nowColidingEntities->next;
    }
    nowColidingEntities = entity->collision.collisionEvents[frameSelect];
    while (wereColidingEntities){
        if (!KON_FindInEntityInstanceList(nowColidingEntities, wereColidingEntities->data)){
            /* Outgoing collision */
            if (call->OnCollisionStop)
                call->OnCollisionStop(scene, entity, (CollisionEvent*)wereColidingEntities->data);
        }
        wereColidingEntities = wereColidingEntities->next;
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

EntityInstance* KON_SpawnEntity(SceneHandle* scene, EntityDescriptor* spawnedEntity, unsigned int layerID, unsigned int X, unsigned int Y) {
    EntityInstance* newInstance = NULL;
    LinkedList* nodePointer = NULL;

    if (!(newInstance = KON_LoadEntity(spawnedEntity)))
        return NULL;

    /* Load the new entity in memory */
    newInstance->pos.x = X;
    newInstance->pos.y = Y;
    newInstance->layerID = layerID;
    newInstance->collision.generateCollisionEvents = true;

    if (newInstance->descriptor->OnSetup)
        newInstance->descriptor->OnSetup(scene, newInstance);

    nodePointer = KON_AppendRefToLinkedList(&scene->entityInstanceList, newInstance);
    
    return ((EntityInstance*)nodePointer->data);
}

/* TODO: Check that again */
void KON_KillEntityInstance(SceneHandle* scene, EntityInstance* entityInstanceToKill) {
    LinkedList** entityInstanceList = &scene->entityInstanceList;
    bool instanceKilled = false;

    while (*entityInstanceList){
        if ((EntityInstance*)(*entityInstanceList)->data == entityInstanceToKill){
            if (instanceKilled){
                return;
            } else {
                /* Free entity instance */
                KON_FreeEntity(entityInstanceToKill);
                KON_DeleteLinkedListNode(entityInstanceList); /* Don't forget to clean the instance's context :3 */
                instanceKilled = true;
            }
        }
        if (!*entityInstanceList)
            break;
        entityInstanceList = &(*entityInstanceList)->next;
    }
}