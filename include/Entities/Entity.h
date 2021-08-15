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

    typedef void (*functPtrEntity)(KONDevice* KDevice, SceneHandle* scene, EntityInstance* entity);

    /* Entity descriptor data type */
    struct EntityDescriptor{
        /* Data */
        char* EntityDesctiptorPath;

        /* Logic */
        functPtrEntity OnSetup;
        functPtrEntity OnEvent;
        functPtrEntity OnFrame;
        functPtrEntity OnExit;
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

    /* Particular instance of an entity */
    struct EntityInstance{
        Entity* commun;
        Vector2d pos;
        unsigned int layerID;
        SDL_Rect boundingBox;
        
        /* Animation instance */
        Uint32 LastFrame;       /* Time at the last frame display */
        unsigned int CurrentFrame;       /* Frame currently being displayed */
        unsigned int PlayingAnimation;   /* The Animation currently playing */
        bool Flip;                      /* Defines if the entity should be drawned flipped */
        bool alimationLoop;

        void* EntityInstanceContext;
    };

    Entity* KON_LoadEntity(DisplayDevice* DDevice, EntityDescriptor* entityToLoad);
    void KON_DrawEntity(DisplayDevice* DDevice, EntityInstance* entity);
    void KON_EntityPlayAnimation(EntityInstance* entity, unsigned int AnimationID, bool reset, bool loop);
    void KON_MoveEntity(SceneHandle* scene, EntityInstance* entityInstancePointer, double X, double Y);
    
#endif