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
    typedef void (*functPtrEntity)(KONDevice* KDevice, SceneHandle* scene, EntityInstance* self);
    typedef void (*functPtrEntityColison)(KONDevice* KDevice, SceneHandle* scene, EntityInstance* self, CollisionEvent* collision);

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
        functPtrEntity OnDraw;
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
    struct EntityInstance{
        EntityDescriptor* descriptor;

        /* "Behind the scene" properties */
        Vector2d lastPos;

        /* Global Properties */
        EntityProperties properties;

        /* Visual properties */
        Sprite entitySprite;
        unsigned int layerID; /* TODO: Should that be a part of the sprite itself ? */
        Vector2d pos; /* Absolute position in space */
        Vector2d mov; /* Relative frame-to-frame movement (reset each frame) */

        /* Colisions properties */
        Collision collision;

        void* EntityInstanceContext;
    };

    EntityInstance* KON_LoadEntity(DisplayDevice* DDevice, EntityDescriptor* entityToLoad);
    void            KON_FreeEntity(EntityInstance* entityToFree);
    void            KON_DrawEntity(DisplayDevice* DDevice, EntityInstance* entity);
    void            KON_PlayEntityAnimation(EntityInstance* entity, unsigned int animationID, bool reset, bool loop);
    void            KON_ProcessEntityCollisionsCalls(KONDevice* KDevice, SceneHandle* scene, EntityInstance* entity);
    void            KON_BoundEntityInstanceToRect(EntityInstance* entity, KON_Rect* rect);
    EntityInstance* KON_SpawnEntity(KONDevice* KDevice, SceneHandle* scene, EntityDescriptor* SpawnedEntity, unsigned int layerID, unsigned int X, unsigned int Y);
    void            KON_KillEntityInstance(SceneHandle* scene, EntityInstance* entityInstanceToKill);
    
#endif