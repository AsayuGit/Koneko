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

#include "Music.h"

Mix_Music* KON_LoadMusic(char FilePath[]){
    Mix_Music* LoadingMusic = NULL;

    if (FilePath){
        LoadingMusic = Mix_LoadMUS(FilePath);
        if (LoadingMusic == NULL)
            fprintf(stderr, "Can't load music %s\n", Mix_GetError());
    }
    
    return LoadingMusic;
}

Mix_Chunk* KON_LoadSoundEffect(char FilePath[]){
    Mix_Chunk* LoadingSoundEffect = NULL;

    LoadingSoundEffect = Mix_LoadWAV(FilePath);
    if (LoadingSoundEffect == NULL)
        fprintf(stderr, "Can't load sound effect %s\n", Mix_GetError());
    return LoadingSoundEffect;
}