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

#include "Collisions.h"

#include "Graphics.h"
#include "CommunFunctions.h"

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

/*
    KON_AddNewCollisionEvent() : Generates a new collision event and appends it to a list of collision events
    INPUT: Node** list                      : CollisionEvent LinkedList pointer
           EntityInstance ** colidingEntity : Pointer to the EntityInstance currently colliding
*/
void KON_AddNewCollisionEvent(LinkedList** list, EntityInstance **colidingEntity){
    CollisionEvent newCollisonEvent;

    if (*list){
        /*printf("EDITTED\n");*/
        ((CollisionEvent*)(*list)->data)->collidingEntitiy = *colidingEntity;
        ((CollisionEvent*)(*list)->data)->entityCollidingPosition = (*colidingEntity)->pos;
        ((CollisionEvent*)(*list)->data)->entityLastPosition = (*colidingEntity)->lastPos;
    } else {
        /*printf("ADDED\n");*/
        newCollisonEvent.collidingEntitiy = *colidingEntity;
        newCollisonEvent.entityCollidingPosition = (*colidingEntity)->pos;
        newCollisonEvent.entityLastPosition = (*colidingEntity)->lastPos;

        KON_appendToList(list, &newCollisonEvent, sizeof(CollisionEvent));
    }
}

void KON_EntityEntityCollisionCheck(KONDevice* KDevice, SceneHandle* scene) {
    SDL_Rect collisionResult;
    EntityInstance *entityA, *entityB;
    LinkedList* nodePointer, *nodePointerB, *nextEntity;
    LinkedList** colidingEntities;
    SDL_Rect entityABoundingBox, entityBBoundingBox;

    nodePointer = scene->entityInstanceList;
    while (nodePointer){
        entityA = ((EntityInstance*)nodePointer->data);
        entityA->collision.collisionFrameSelect ^= 1;
        colidingEntities = &(entityA->collision.collisionEvents[entityA->collision.collisionFrameSelect]);

        nodePointerB = scene->entityInstanceList;
        while (nodePointerB){
            if (nodePointer != nodePointerB){ /* Prevent an entity from coliding with itself */
                entityB = (EntityInstance*)nodePointerB->data;

                /* Collision test */
                entityABoundingBox = KON_GetRectVectAddition(entityA->boundingBox, entityA->mov);
                entityBBoundingBox = KON_GetRectVectAddition(entityB->boundingBox, entityB->mov);
                if (SDL_IntersectRect(&entityABoundingBox, &entityBBoundingBox, &collisionResult) && entityB->collision.generateCollisionEvents) {
                    /* Add entityB to the CollisionEvent list */
                    KON_AddNewCollisionEvent(colidingEntities, &entityB);
                    colidingEntities = &((*colidingEntities)->next);
                }
            }
            nodePointerB = nodePointerB->next;
        }

        /* Free the unused section of the list */
        KON_FreeList(colidingEntities);

        nextEntity = nodePointer->next;
        if (entityA->collision.generateCollisionEvents)
            KON_ProcessEntityCollisionsCalls(KDevice, scene, entityA);
        
        nodePointer = nextEntity;
    }
}

void KON_EntityCollisions(KONDevice* KDevice, SceneHandle* scene) {
    EntityInstance *entityInstancePointer;
    LinkedList* nodePointer;

    /* Entity / Scene Collisions */
    nodePointer = scene->entityInstanceList;
    while (nodePointer){
        entityInstancePointer = ((EntityInstance*)nodePointer->data);

        KON_EntitySceneCollisionCheck(scene, entityInstancePointer);
        nodePointer = nodePointer->next;
    }

    /* Entity / Entity Collisions */
    KON_EntityEntityCollisionCheck(KDevice, scene);

    /* Apply/raz entity movement */
    nodePointer = scene->entityInstanceList;
    while (nodePointer){
        entityInstancePointer = ((EntityInstance*)nodePointer->data);
        
        entityInstancePointer->lastPos = entityInstancePointer->pos;

        entityInstancePointer->pos.x += entityInstancePointer->mov.x;
        entityInstancePointer->pos.y += entityInstancePointer->mov.y;

        entityInstancePointer->mov = KON_InitVector2d(0.0, 0.0);

        nodePointer = nodePointer->next;
    }
}

/*
    KON_GetEntityCollisionNormal() : Returns an Unit Vector describing the direction of the collision
    INPUT: EntityInstance* self        : Pointer to the EntityInstance wishing to get this information
           CollisionEvent collision[2] : CollisionEvent present and past
           bool frameSelect            : Determines which of the CollisionEvent is present and which is past
    OUTPUT: Vector2d                   : Unit Vector of the collision
*/
Vector2d KON_GetEntityCollisionNormal(EntityInstance* self, CollisionEvent collision[2], bool frameSelect) {
    Vector2d collisionUnitVector;

    /* Curent Position - Last Position */
    collisionUnitVector = KON_GetVectSubstraction(
        KON_GetVectSubstraction(collision[frameSelect].entityCollidingPosition, self->pos),
        KON_GetVectSubstraction(collision[frameSelect].entityLastPosition, self->lastPos)
    );

    /* Return Normalized vector */
    return KON_GetVectScalarDivision(collisionUnitVector, KON_GetVectNorm(collisionUnitVector));
}

Direction KON_GetEntityCollisionDirection(EntityInstance* self, CollisionEvent collision[2], bool frameSelect) {
    return KON_GetDirectionFromVector(KON_GetEntityCollisionNormal(self, collision, frameSelect));
}