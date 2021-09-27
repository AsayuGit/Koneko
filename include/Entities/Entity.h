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

    #include "Types.h"
    #include "Animation.h"
    typedef struct EntityInstance EntityInstance;
    typedef struct EntityDescriptor EntityDescriptor;
    #include "Scene.h"
    #include "Collisions.h"

    typedef void (*functPtrEntityFree)(EntityInstance* entity);
    typedef void (*functPtrEntity)(KONDevice* KDevice, SceneHandle* scene, EntityInstance* entity);
    typedef void (*functPtrEntityColison)(KONDevice* KDevice, SceneHandle* scene, EntityInstance* self, EntityInstance* colidingEntity);

    /* Entity descriptor data type */
    struct EntityDescriptor{
        /* Data */
        char* EntityDesctiptorPath;

        /* Logic */
        functPtrEntity OnSetup;
        functPtrEntityFree OnExit;
        functPtrEntity OnEvent;
        functPtrEntity OnFrame;
        functPtrEntityColison OnCollisionStart;
        functPtrEntityColison OnCollisionStay;
        functPtrEntityColison OnCollisionStop;
    };

    /* Commun data between instances */
    typedef struct {
        /* Data */
        char* entityName;
        SDL_Texture* EntityTexture;
        Animation* entityAnimations;
        bool isSolid;

        /* Logic */
        EntityDescriptor* descriptor;
    } Entity;

    /*  TODO: Hey here's an idea : Lets have two EntityInstance Types,
        one that contains the behind the scenes info about an instance
        and another that contains the info we want exposed to the user.
        Like EntityInstance is the behind the scene and EntityHandle
        is on the fronthand, with EntityInstance being a parent of 
        EntityHandle. */

    /* Particular instance of an entity */
    struct EntityInstance{
        Entity* commun;

        /* Instance properties */
        Vector2d pos; /* Absolute position in space */
        Vector2d mov; /* Relative frame-to-frame movement (reset each frame) */
        unsigned int layerID;
        SDL_Rect boundingBox; /* Bounding box of the current entity instance (Updated each frames) */
        bool isVisible;
        
        /* Animation instance */
        Uint32 LastFrame;       /* Time at the last frame display */
        unsigned int CurrentFrame;       /* Frame currently being displayed */
        unsigned int PlayingAnimation;   /* The Animation currently playing */
        bool Flip;                      /* Defines if the entity should be drawned flipped */
        bool alimationLoop;

        /* Colisions properties */
        Collision collision;

        void* EntityInstanceContext;
    };

    Entity* KON_LoadEntity(DisplayDevice* DDevice, EntityDescriptor* entityToLoad);
    void    KON_FreeEntityInstance(EntityInstance* entityInstanceToFree);
    void    KON_FreeEntity(Entity* entityToFree);
    void    KON_DrawEntity(DisplayDevice* DDevice, EntityInstance* entity);
    void    KON_EntityPlayAnimation(EntityInstance* entity, unsigned int AnimationID, bool reset, bool loop);
    void    KON_ProcessEntityCollisionsCalls(KONDevice* KDevice, SceneHandle* scene, EntityInstance* entity);
    
#endif