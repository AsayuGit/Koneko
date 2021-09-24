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

void KON_AddEntityToCollisionList(Node** list, EntityInstance **colidingEntity){
    if (*list){
        /*printf("EDITTED\n");*/
        *((EntityInstance**)(*list)->data) = *colidingEntity;
    } else {
        /*printf("ADDED\n");*/
        KON_appendToList(list, colidingEntity, sizeof(EntityInstance*));
    }
}

void KON_EntityEntityCollisionCheck(KONDevice* KDevice, SceneHandle* scene) {
    SDL_Rect collisionResult;
    EntityInstance *entityA, *entityB;
    Node* nodePointer, *nodePointerB;
    Node** colidingEntities;

    nodePointer = scene->entityInstanceList;
    while (nodePointer){
        entityA = ((EntityInstance*)nodePointer->data);
        entityA->collision.colisionFrameSelect ^= 1;
        colidingEntities = &(entityA->collision.colidingEntities[entityA->collision.colisionFrameSelect]);

        nodePointerB = scene->entityInstanceList;
        while (nodePointerB){
            if (nodePointer != nodePointerB){
                entityB = (EntityInstance*)nodePointerB->data;

                if (SDL_IntersectRect(&entityA->boundingBox, &entityB->boundingBox, &collisionResult)) {
                    KON_AddEntityToCollisionList(colidingEntities, &entityB);
                    colidingEntities = (Node**)&((*colidingEntities)->next);
                }
            }
            nodePointerB = (Node*)nodePointerB->next;
        }
        KON_FreeList(colidingEntities);

        KON_ProcessEntityCollisionsCalls(KDevice, scene, entityA);

        /* TEMP */
        /*
        if (KON_ListCount(entityA->collision.colidingEntities[entityA->collision.colisionFrameSelect])){
            printf("Entity %s coliding with %u other entities %s \n", 
                entityA->commun->entityName, 
                KON_ListCount(entityA->collision.colidingEntities[entityA->collision.colisionFrameSelect]),
                 (*(EntityInstance**)entityA->collision.colidingEntities[entityA->collision.colisionFrameSelect]->data)->commun->entityName);
        }*/

        nodePointer = (Node*)nodePointer->next;
    }
}

void KON_EntityColisions(KONDevice* KDevice, SceneHandle* scene) {
    EntityInstance *entityInstancePointer;
    Node* nodePointer;

    /* Entity / Scene Collisions */
    nodePointer = scene->entityInstanceList;
    while (nodePointer){
        KON_EntitySceneCollisionCheck(scene, (EntityInstance*)nodePointer->data);
        nodePointer = (Node*)nodePointer->next;
    }

    /* Entity / Entity Collisions */
    KON_EntityEntityCollisionCheck(KDevice, scene);

    /* Apply/raz entity movement */
    nodePointer = scene->entityInstanceList;
    while (nodePointer){
        entityInstancePointer = ((EntityInstance*)nodePointer->data);

        entityInstancePointer->pos.x += entityInstancePointer->mov.x;
        entityInstancePointer->pos.y += entityInstancePointer->mov.y;

        entityInstancePointer->mov = InitVector2d(0.0, 0.0);

        nodePointer = (Node*)nodePointer->next;
    }

    /* TODO: Reset entity->mov when done */
}