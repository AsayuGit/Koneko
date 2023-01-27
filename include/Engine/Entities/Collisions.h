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

#ifndef _COLLISIONS_H
#define _COLLISIONS_H

    #include "Bool.h"
    #include "LinkedList.h"
    #include "Vector.h"

    /*
        struct Collision : Contains all properties regarding an entity's collision
    */
    typedef struct {
        bool generateCollisionEvents;
        bool collisionFrameSelect;
        LinkedList* collisionEvents[2];
    } Collision;

    typedef struct CollisionEvent CollisionEvent;

    #include "Scene.h"
    #include "Entity.h"

    /*
        struct CollisonEvent : Describe a collision event
    */
    struct CollisionEvent {
        Vector2d entityLastPosition;
        Vector2d entityCollidingPosition;
        Direction collisionDirection;
        bool entityGenerateCollisionEvents;
        EntityInstance* collidingEntitiy;
    };

    /*
        SUMMARY : Computes entity colisions (Entity/Scene) + (Entity/Entity)
        INPUT   : SceneHandle* scene : The scene where the entities should be processed.
    */
    void     KON_ProcessEntityCollisions(SceneHandle* scene);

    /*
        SUMMARY : Returns an Unit Vector describing the direction of the collision
        INPUT   : EntityInstance* self        : Pointer to the EntityInstance wishing to get this information
        INPUT   : CollisionEvent collision[2] : CollisionEvent present and past
        INPUT   : bool frameSelect            : Determines which of the CollisionEvent is present and which is past
        OUTPUT  : Vector2d                   : Unit Vector of the collision
    */
    Vector2d KON_GetEntityCollisionNormal(EntityInstance* self, CollisionEvent collision[2], bool frameSelect);

#endif
