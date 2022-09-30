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

#ifndef _MAP_H
#define _MAP_H

    #include "LinkedList.h"
    #include "Rect.h"
    #include "DisplayList.h"
    #include "Animation.h"

    typedef struct MapLayer MapLayer;

    #include "Renderer.h"

    struct MapLayer {
        KON_Renderers layerRenderer;
        union {
            KON_Surface* gpuSide;
            struct {
                uint32_t* pixelData;
                unsigned int width;
                unsigned int height;
            } cpuSide;
        } texture;
        void* layerData;
        Vector2d pos;
        KON_Rect boundingBox;
        LinkedList* entityInstanceList;
        DisplayList* displayList;

        size_t nbOfKeyFrameAnimations;
        unsigned int playingAnimation;
        KON_KeyFrameAnimation* keyFrameAnimationArray;

        size_t effectBufferPitch;
        uint32_t* effectBuffer; /* Used to rendrer CPU driven effects to the screen. Most notably the raycasting output */
        size_t zBufferPitch;
        double* zBuffer;      /* Used to store depth values through a scene */
        bool shown;
    };

    typedef struct{
        char* MapFilePath;
        unsigned int nbOfLayers;
        MapLayer* MapLayer; /* Map Layer array */
    } Map;

    /*
        SUMMARY : Loads a Map form disk.
        INPUT   : char** mapFilePath : The path to the map to load.
        OUTPUT  : Map*              : The newly loaded map (or NULL on error).
    */
    Map* KON_LoadMap(char* mapFilePath);

    /*
        SUMMARY : Free a previously loaded map.
        INPUT  : Map* map : The map to free.
    */
    void KON_FreeMap(Map** map);

    /*
        SUMMARY : Adds a prite to a layer
    */
    void KON_AddSpriteToLayer(Map* map, Sprite* sprite, unsigned int layerID, unsigned int priority, double x, double y);

    /*
        SUMMARY : Start an animation on a layer.
        INPUT   : Map* map             : The map the layer is part of.
        INPUT   : unsigned int layerID : The Nb of the layer to animate.
        INPUT   : unsigned int animID  : The NB of the animation to play.
    */
    void KON_PlayLayerAnimation(Map* map, unsigned int layerID, unsigned int animID);

    void KON_DrawMap(Map* map);

#endif