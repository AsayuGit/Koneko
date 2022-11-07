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

#ifndef _SCENE_H
#define _SCENE_H
    #include <stdarg.h>
    
    typedef struct SceneHandle SceneHandle;
    typedef void (*functPtrScene)(SceneHandle* scene);
    typedef void (*functPtrOnDisplay)(SceneHandle* scene, unsigned int layerID);
    typedef void (*functPtrVaScene)(SceneHandle* scene, va_list args);


    #include "Map.h"
    #include "Entity.h"
    #include "LinkedList.h"

    struct SceneHandle {
        Map* WorldMap;
        void* sceneInstanceContext;
    };
    /* Scene desctiptor data type */
    typedef struct {
        /* Data */
        char* WorldMapPath;

        /* Logic */
        functPtrVaScene OnSetup;
        functPtrScene OnExit;
        functPtrScene OnEvent;
        functPtrScene OnFrame;
        functPtrOnDisplay OnDisplay;
    } SceneDescriptor;

    /*
        SUMMARY : Starts up a Scene from a scene descroptor
        INPUT   : SceneDescriptor* scenePointer : The scene to start up.
        INPUT   : ...                           : Scene specific arguments
        OUTPUT  : int                           : Success.
    */
    int KON_StartScene(SceneDescriptor* scenePointer, ...);

    /*
        SUMMARY : End a running scene.
        INPUT   : SceneHandle* scene.
    */
    void KON_EndScene(SceneHandle* scene);

#endif