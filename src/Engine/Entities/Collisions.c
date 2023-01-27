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

#include "Collisions.h"
#include "Entity.h"
#include "Graphics.h"
#include "CommunFunctions.h"
#include "TileMap.h"

/* FIXME: Uses boundingboxes instead of raw position to check scene collisions */
static void KON_EntityLayerCollisionCheck(MapLayer* layer) {
    EntityInstance* entityInstancePointer;
    LinkedList* entityInstanceList;
    Vector2i entityNewTile;
    TileMap* tileMap;
    unsigned int tileSize;

    if (layer->layerRenderer != RENDER_2D_TILEMAP)
        return;
    
    tileMap = (TileMap*)layer->layerData;
    tileSize = tileMap->TileSize;

    entityInstanceList = layer->entityInstanceList;
    while (entityInstanceList) {
        entityInstancePointer = (EntityInstance*)entityInstanceList->data;

        /* Only process collision if the entity is actually solid */
        if (!entityInstancePointer->properties.isSolid) {
            entityInstanceList = entityInstanceList->next;
            continue;
        }

        entityNewTile.x = (int)((entityInstancePointer->pos.x + entityInstancePointer->mov.x) / tileSize);
        entityNewTile.y = (int)((entityInstancePointer->pos.y + entityInstancePointer->mov.y) / tileSize);
        
        /* X Collisions */
        /* FIXME: Check all layers ? */
        if (KON_IsTileMapTileSolid(tileMap, entityNewTile.x, (int)entityInstancePointer->pos.y / tileSize, 0, NULL)) {
            if (entityInstancePointer->mov.x > 0){
                entityInstancePointer->mov.x = floor((entityNewTile.x * tileSize - 1) - entityInstancePointer->pos.x) + 1;
            } else {
                entityInstancePointer->mov.x = ((entityNewTile.x + 1) * tileSize) - entityInstancePointer->pos.x;
            }
            /*printf("DEBUG: EntitySceneCollision on X axis\n");*/
        }
        
        /* Y Collisions */
        /* FIXME: Check all layers ? */
        if (KON_IsTileMapTileSolid(tileMap, (unsigned int)(entityInstancePointer->pos.x / tileSize), (unsigned int)(entityNewTile.y), 0, NULL)) {
            if (entityInstancePointer->mov.y > 0){
                entityInstancePointer->mov.y = floor((entityNewTile.y * tileSize - 1) - entityInstancePointer->pos.y) + 1;
            } else {
                entityInstancePointer->mov.y = ((entityNewTile.y + 1) * tileSize) - entityInstancePointer->pos.y;
            }
            /*printf("DEBUG: EntitySceneCollision on Y axis\n");*/
        }
        
        entityInstanceList = entityInstanceList->next;
    }
}

static void KON_EntitySceneCollisionCheck(SceneHandle* scene) {
    unsigned int i;

    for (i = 0; i < scene->WorldMap->nbOfLayers; i++)
        KON_EntityLayerCollisionCheck(scene->WorldMap->MapLayer + i);
}

/*
    KON_AppendNewCollisionEvent() : Generates a new collision event and appends it to a list of collision events
    INPUT: Node** list                      : CollisionEvent LinkedList pointer
    INPUT: EntityInstance ** colidingEntity : Pointer to the EntityInstance currently colliding
*/
void KON_AppendNewCollisionEvent(LinkedList** list, EntityInstance **colidingEntity, Direction collisonDirection){
    CollisionEvent newCollisonEvent;

    while (*list) {
        /* If the colidingEntity already generted a CollisionEvent ABORT */
        if (((CollisionEvent*)(*list)->data)->collidingEntitiy == *colidingEntity)
            return;

        /* If not continue and find a free spot */
        list = &(*list)->next;
    }

    newCollisonEvent.collidingEntitiy = *colidingEntity;
    newCollisonEvent.entityCollidingPosition = (*colidingEntity)->pos;
    newCollisonEvent.collisionDirection = collisonDirection;
    newCollisonEvent.entityLastPosition = (*colidingEntity)->lastPos;
    newCollisonEvent.entityGenerateCollisionEvents = (*colidingEntity)->collision.generateCollisionEvents;

    KON_AppendToLinkedList(list, &newCollisonEvent, sizeof(CollisionEvent));
}

Direction KON_GetEntityCollisionDirection(Vector2d entityAPos, Vector2d entityBPos, KON_Rect collisionResult) {
    if (entityAPos.x < entityBPos.x) {
        if (entityAPos.y < entityBPos.y) {
            /* LOW RIGHT */
            return (collisionResult.w < collisionResult.h) ? Right : Down;
        } else {
            /* HIGH RIGHT */
            return (collisionResult.w < collisionResult.h) ? Right : Up;
        }
    } else {
        if (entityAPos.y < entityBPos.y) {
            /* LOW LEFT */
            return (collisionResult.w < collisionResult.h) ? Left : Down;
        } else {
            /* HIGH LEFT */
            return (collisionResult.w < collisionResult.h) ? Left : Up;
        }
    }
}

/*
    SUMMARY : Check collisions between all entities
    INPUT   : SceneHanle* scene : Pointer to the current scene
*/
static void KON_EntityEntityCollisionCheck(SceneHandle* scene) {
    KON_Rect collisionResult;
    EntityInstance *entityA, *entityB;
    LinkedList* nodePointer, *nodePointerB;
    KON_Rect entityABoundingBox, entityBBoundingBox;
    MapLayer* layer;
    unsigned int i;

    for (i = 0; i < scene->WorldMap->nbOfLayers; i++) {
        layer = scene->WorldMap->MapLayer + i;
        nodePointer = layer->entityInstanceList;
        while (nodePointer){
            entityA = ((EntityInstance*)nodePointer->data);

            nodePointerB = layer->entityInstanceList;
            while (nodePointerB){
                if (nodePointer == nodePointerB) { /* Prevent an entity from coliding with itself */
                    nodePointerB = nodePointerB->next;
                    continue;
                }
                entityB = (EntityInstance*)nodePointerB->data;

                /* Collision test */
                KON_RectPlusVect(entityABoundingBox, entityA->entitySprite->boundingBox, entityA->mov);
                KON_RectPlusVect(entityBBoundingBox, entityB->entitySprite->boundingBox, entityB->mov);
                if (KON_GetRectRectIntersection(&entityABoundingBox, &entityBBoundingBox, &collisionResult) && entityB->collision.generateCollisionEvents) {
                    
                    if (entityA->properties.isSolid) {
                        /* Prevent entity A from going into entity B */
                        /* We substract to the "Position" the smallest dimention of the collisionResult */

                        if (collisionResult.w < collisionResult.h) {
                            entityA->mov.x -= collisionResult.w;
                        } else {
                            entityA->mov.y -= collisionResult.h;
                        }
                    }

                    /* Append CollisionEvents */
                    KON_AppendNewCollisionEvent(entityA->collision.collisionEvents + entityA->collision.collisionFrameSelect, &entityB, KON_GetEntityCollisionDirection(entityA->pos, entityB->pos, collisionResult)); /* For entity A */
                    KON_AppendNewCollisionEvent(entityB->collision.collisionEvents + entityB->collision.collisionFrameSelect, &entityA, KON_GetEntityCollisionDirection(entityB->pos, entityA->pos, collisionResult)); /* For entity B */
                }
                nodePointerB = nodePointerB->next;
            }

            nodePointer = nodePointer->next;
        }
    }
}

/* Swap and initialise a new collision frame */
static void KON_InitEntityCollisionFrame(SceneHandle* scene) {
    EntityInstance *entityInstance;
    LinkedList* entityInstanceList;
    unsigned int i;
    
    /* For each map plane */
    for (i = 0; i < scene->WorldMap->nbOfLayers; i++) {
        entityInstanceList = scene->WorldMap->MapLayer[i].entityInstanceList;
        while (entityInstanceList){
            entityInstance = ((EntityInstance*)entityInstanceList->data);

            /* Present collision frame becomes past collision frame */
            entityInstance->collision.collisionFrameSelect ^= 1;

            KON_FreeLinkedList(entityInstance->collision.collisionEvents + entityInstance->collision.collisionFrameSelect);
            entityInstanceList = entityInstanceList->next;
        }
    }
}

void KON_ProcessEntityCollisions(SceneHandle* scene) {
    /* Entity -> Scene */
    KON_EntitySceneCollisionCheck(scene); /* Doesn't generate collision events */

    /* Entity -> Entity */
    KON_InitEntityCollisionFrame(scene);
    KON_EntityEntityCollisionCheck(scene);
    KON_ProcessEntityCollisionsCalls(scene);
}

Vector2d KON_GetEntityCollisionNormal(EntityInstance* self, CollisionEvent collision[2], bool frameSelect) {
    Vector2d collisionUnitVector, collidingPosition, lastPosition;

    /* Curent Position - Last Position */
    collidingPosition = KON_GetVectSubstraction(&collision[frameSelect].entityCollidingPosition, &self->pos);
    lastPosition = KON_GetVectSubstraction(&collision[frameSelect].entityLastPosition, &self->lastPos);
    collisionUnitVector = KON_GetVectSubstraction(&collidingPosition, &lastPosition);

    /* Return Normalized vector */
    return KON_GetVectScalarDivision(collisionUnitVector, KON_GetVectNorm(collisionUnitVector));
}
