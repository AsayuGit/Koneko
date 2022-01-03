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

#ifndef _LOAD_H
#define _LOAD_H
	
	#ifdef _XBOX
		#include <libxml/parser.h>
	#else
		#include <libxml2/libxml/parser.h>
	#endif
    
    #include "Types.h"
    #include "Map.h"
    
    /* Surface */
    enum {
        SURFACE_OPAQUE = 0,
        SURFACE_ALPHA = 1,
        SURFACE_KEYED = 2
    };

    SDL_Texture* KON_LoadSurface(char FilePath[], DisplayDevice* Device, Uint32 ColorKey, Uint8 flags);
    void         KON_FreeSurface(SDL_Texture* surface);
    SDL_Texture* KON_CreateTargetSurface(DisplayDevice* DDevice, int w, int h);

    /* Fonts */
    BitmapFont*  KON_LoadBitmapFont(char FilePath[], DisplayDevice* DDevice, Uint32 FontColorKey);

    /* Music */
    Mix_Music*   KON_LoadMusic(char FilePath[]);
    Mix_Chunk*   KON_LoadSoundEffect(char FilePath[]);

    /* Documents */
    xmlDoc*      KON_LoadXml(char* filePath);

    /* Maps */
    Map*         KON_LoadMap(DisplayDevice* DDevice, char* MapFilePath);
    void         KON_SaveTileMap(Map* MapToSave);
#endif