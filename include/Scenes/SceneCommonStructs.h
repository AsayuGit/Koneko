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

#ifndef _SCENECOMMONSTRUCTS_H
#define _SCENECOMMONSTRUCTS_H

    #include "include.h"
    #include "Dialogue.h"
    #include "ButtonsStructs.h"

    typedef struct{
        unsigned int NbOfAnimStates;
        Vector2d* AnimStates;
        int* AnimRuntime;
        Vector2d* AnimRange;
        SDL_Rect AnimRegion;
    } BGAnimation;

    typedef struct{
        /* Background Texture properties */
        SDL_Texture* Surface;           /* Background Surface */
        int TileID;
        SDL_Rect SrcRect;
        
        /* Background Animation properties */
        int PlayingAnimation;
        Uint32 StartFrame;
        int CurrentState;
        int AnimOffset;
        unsigned int nbOfAnimations;
        BGAnimation* Animation;

        /* Scenes index */
        unsigned int nbOfCoordinates;
        Vector2i* ScenesCoordinates;

        /* Metadata */
        bool* EndSignal;                /* Reference */
        bool EndSignalValue;
        char Flipped;
        int ObjectLayerOffset;
        
        bool Shown;
    } BackgroundContext;

#endif