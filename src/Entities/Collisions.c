#include "Collisions.h"

#include "Graphics.h"

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
    KON_AddEntityToCollisionList() : Adds an EntityInstance to the list of coliding entities of an entity
    INPUT: Node** list                      [Pointer to a list of coliding entities of an EntityInstance]
           EntityInstance ** colidingEntity [Pointer to the EntityInstance currently colliding]

*/
void KON_AddEntityToCollisionList(LinkedList** list, EntityInstance **colidingEntity){
    CollisionEvent newCollisonEvent;
    if (*list){
        /*printf("EDITTED\n");*/
        ((CollisionEvent*)(*list)->data)->collidingEntitiy = *colidingEntity;
    } else {
        /*printf("ADDED\n");*/
        newCollisonEvent.collidingEntitiy = *colidingEntity;
        KON_appendToList(list, &newCollisonEvent, sizeof(CollisionEvent));
    }
}

void KON_EntityEntityCollisionCheck(KONDevice* KDevice, SceneHandle* scene) {
    SDL_Rect collisionResult;
    EntityInstance *entityA, *entityB;
    LinkedList* nodePointer, *nodePointerB, *nextEntity;
    LinkedList** colidingEntities;

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
                if (SDL_IntersectRect(&entityA->boundingBox, &entityB->boundingBox, &collisionResult) && entityB->collision.generateCollisionEvents) {
                    KON_AddEntityToCollisionList(colidingEntities, &entityB);
                    colidingEntities = &((*colidingEntities)->next);
                }
            }
            nodePointerB = nodePointerB->next;
        }

        KON_FreeList(colidingEntities);

        nextEntity = nodePointer->next;
        if (entityA->collision.generateCollisionEvents)
            KON_ProcessEntityCollisionsCalls(KDevice, scene, entityA);
        
        nodePointer = nextEntity;
    }
}

void KON_EntityColisions(KONDevice* KDevice, SceneHandle* scene) {
    EntityInstance *entityInstancePointer;
    LinkedList* nodePointer;

    /* Entity / Scene Collisions */
    nodePointer = scene->entityInstanceList;
    while (nodePointer){
        KON_EntitySceneCollisionCheck(scene, (EntityInstance*)nodePointer->data);
        nodePointer = nodePointer->next;
    }

    /* Entity / Entity Collisions */
    KON_EntityEntityCollisionCheck(KDevice, scene);

    /* Apply/raz entity movement */
    nodePointer = scene->entityInstanceList;
    while (nodePointer){
        entityInstancePointer = ((EntityInstance*)nodePointer->data);

        entityInstancePointer->pos.x += entityInstancePointer->mov.x;
        entityInstancePointer->pos.y += entityInstancePointer->mov.y;

        entityInstancePointer->mov = KON_InitVector2d(0.0, 0.0);

        nodePointer = nodePointer->next;
    }
}