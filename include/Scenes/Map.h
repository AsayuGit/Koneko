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
    #include "Surface.h"
    #include "Rect.h"
    #include "DisplayList.h"

    enum {
        KON_LAYER_BITMAP = 1,
        KON_LAYER_TILEMAP
    }; /* TODO: KON_LayerType */

    typedef struct{
        unsigned int layerType;
        void* layerData;
        Vector2d pos;
        KON_Rect boundingBox;
        LinkedList* entityInstanceList;
        DisplayList* displayList;
        bool shown;
    } MapLayer;

    typedef struct{
        char* MapFilePath;
        unsigned int nbOfLayers;
        MapLayer* MapLayer;
    } Map;

    /*
        SUMMARY : Loads a Map form disk.
        INPUT   : char* mapFilePath : The path to the map to load.
        OUTPUT  : Map*              : The newly loaded map (or NULL on error).
    */
    Map* KON_LoadMap(char* mapFilePath);

    void KON_AddSpriteToLayer(Map* map, Sprite* sprite, unsigned int layerID, unsigned int priority);

    void KON_DrawMap(Map* map);

#endif