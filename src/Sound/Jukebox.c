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

#include "Jukebox.h"
#include "Types.h"
#include "Load.h"

static Mix_Music* Track_INTRO = NULL;
static Mix_Music* Track_LOOP = NULL;

/* MUSIC QUEUE SYSTEM */

void KON_PlayTrackFromDisk(char* IntroPath, char* LoopPath){
    KON_StopTrack();

    if (Track_INTRO){
        Mix_FreeMusic(Track_INTRO);
        Track_INTRO = NULL;
    }

    if (Track_LOOP){
        Mix_FreeMusic(Track_LOOP);
        Track_LOOP = NULL;
    }
    
    if (IntroPath)
        Track_INTRO = KON_LoadMusic(IntroPath);
    if (LoopPath){
        Track_LOOP = KON_LoadMusic(LoopPath);
    }

    if (Track_INTRO)
        Mix_PlayMusic(Track_INTRO, 1);
}

void KON_MusicDaemon(void){
    if (!Mix_PlayingMusic() && Track_LOOP){
        Mix_PlayMusic(Track_LOOP, -1);
    }
}

void KON_StopTrack(void){
    Mix_HaltMusic();
}