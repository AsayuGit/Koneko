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

#ifndef _ENTITY_H
#define _ENTITY_H

    #include "Sprite.h"
    #include "Animation.h"
    typedef struct EntityInstance EntityInstance;
    typedef struct EntityDescriptor EntityDescriptor;
    typedef struct EntityProperties EntityProperties;
    #include "Scene.h"
    #include "Collisions.h"

    typedef void (*functPtrEntityFree)(EntityInstance* entity);
    typedef void (*functPtrEntity)(SceneHandle* scene, MapLayer* layer, EntityInstance* self);
    typedef void (*functPtrEntityColison)(SceneHandle* scene, MapLayer* layer, EntityInstance* self, CollisionEvent* collision);

    struct EntityProperties {
        bool isSolid;
    };

    /* Entity descriptor data type */
    struct EntityDescriptor {
        /* Data */
        char* spriteXmlPath;
        char* entityName;
        EntityProperties properties;

        /* Logic */
        functPtrEntity OnSetup;
        functPtrEntityFree OnExit;
        functPtrEntity OnEvent;
        functPtrEntity OnFrame;
        functPtrEntityColison OnCollisionStart;
        functPtrEntityColison OnCollisionStay;
        functPtrEntityColison OnCollisionStop;
    };

    /*  TODO: Hey here's an idea : Lets have two EntityInstance Types,
        one that contains the behind the scenes info about an instance
        and another that contains the info we want exposed to the user.
        Like EntityInstance is the behind the scene and EntityHandle
        is on the fronthand, with EntityInstance being a parent of 
        EntityHandle. */

    /* Particular instance of an entity */
    struct EntityInstance {
        EntityDescriptor* descriptor;

        bool enabled;

        /* "Behind the scene" properties */
        Vector2d lastPos;

        /* Global Properties */
        EntityProperties properties;

        /* Visual properties */
        Sprite* entitySprite;
        Vector2d pos; /* Absolute position in space */
        Vector2d mov; /* Relative frame-to-frame movement (reset each frame) */

        /* Colisions properties */
        Collision collision;

        void* EntityInstanceContext;
    };

    /*
        SUMMARY : Loads an entity from an EntityDescriptor.
        INPUT   : EntityDescriptor* entityToLoad : The EntityDescriptor to load the entity from.
        OUTPUT  : EntityInstance*                : The resulting loaded EntityInstance.
    */
    EntityInstance* KON_LoadEntity(EntityDescriptor* entityToLoad);

    /*
        SUMMARY : Frees a previously loaded EntityInstance.
        INPUT   : EntityInstance* entityToFree : The EntityInstance to free.
    */
    void            KON_FreeEntity(EntityInstance* entityToFree);

    void            KON_UpdateEntityPosition(EntityInstance* entity);

    /*
        SUMMARY : Starts a new animation for an entity.
        INPUT   : EntityInstance* entity   : The EntityInstance to start the animation for.
        INPUT   : unsigned int animationID : The animation to play.
        INPUT   : bool reset               : If the animation should restart from the begining.
        INPUT   : bool loop                : If the anumation should loop.
    */
    void            KON_PlayEntityAnimation(EntityInstance* entity, unsigned int animationID, bool reset, bool loop);

    /*
        SUMMARY : Calls the relevant function callbacks for the last processed collision.
    */
    void            KON_ProcessEntityCollisionsCalls(SceneHandle* scene);

    /*
        SUMMARY : Prevent an entity form going outside a specified rect.
        INPUT   : EntityInstance* entity : The entity to restrict.
        INPUT   : KON_Rect* rect         : The rect the entity should be restricted by.
    */
    void            KON_BoundEntityInstanceToRect(EntityInstance* entity, KON_Rect* rect);

    /*
        SUMMARY : Spawns a new entity instance into the scene.
        INPUT   : SceneHandle* scene              : The scene the new entity should be spawned in.
        INPUT   : EntityDescriptor* spawnedEntity : The EntityDescriptor of the entity to be spawned.
        INPUT   : unsigned int layerID            : The layer the new entity should be spawned on.
        INPUT   : unsigned int priority           : The display priority the entity should use amongst its layer.
        INPUT   : double x, y                     : The new entity's coordinates.
        OUTPUT  : EntityInstance*                 : The spawned EntityInstance (or NULL on error).
    */
    EntityInstance* KON_SpawnEntity(SceneHandle* scene, EntityDescriptor* spawnedEntity, unsigned int layerID, unsigned int priority, double x, double y);

    /*
        SUMMARY : Kills a previously spawned entity instance.
        INPUT   : SceneHandle* scene                   : The scene in which the entity should be killed.
        INPUT   : EntityInstance* entityInstanceToKill : The EntityInstance to be killed.
    */
    void            KON_KillEntityInstance(SceneHandle* scene, EntityInstance* entityInstanceToKill);
    
#endif