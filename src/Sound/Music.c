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

#include "Music.h"
#include "RessourceManager.h"
#include "Log.h"
#include "API.h"

#include <stddef.h>

struct KON_Music {
    Mix_Music* introTrack;
    Mix_Music* loopTrack;
};

static KON_Music currentTrack;
static int trackLoops;

#define KON_LoadRawMusic(FilePath) Mix_LoadMUS(FilePath)
#define KON_FreeRawMusic(music) Mix_FreeMusic(music)

static Mix_Music* KON_LoadMusicTrack(char* filePath) {
    Mix_Music* loadedTrack;

    if (!filePath)
        return NULL;

    if ((loadedTrack = KON_GetManagedRessource(filePath, RESSOURCE_MUSIC)))
        return loadedTrack;

    if (!(loadedTrack = KON_LoadRawMusic(filePath))) {
        KON_SystemMsg("(KON_LoadMusicTrack) Can't load music file : ", MESSAGE_ERROR, 2, filePath, Mix_GetError());
        return NULL;
    }

    KON_AddManagedRessource(filePath, RESSOURCE_MUSIC, loadedTrack);
    return loadedTrack;
}

static void KON_LoadMusic(char* introPath, char* loopPath) {

    if (!loopPath) {
        KON_SystemMsg("(KON_LoadMusic) No path !", MESSAGE_ERROR, 0);
        return;
    }

    currentTrack.introTrack = (introPath) ? KON_LoadMusicTrack(introPath) : NULL;
    currentTrack.loopTrack = KON_LoadMusicTrack(loopPath);
}

static void KON_FreeMusic(KON_Music* music) {
    if (!music)
        return;

    if (music->introTrack) {
        KON_FreeRawMusic(KON_FreeManagedRessourceByRef(music->introTrack));
    }

    if (music->loopTrack) {
        KON_FreeRawMusic(KON_FreeManagedRessourceByRef(music->loopTrack));
    }

    /* free(music) */
}

void KON_PlayMusic(char* introPath, char* loopPath, int loops) {
    KON_FreeMusic(&currentTrack);
    KON_LoadMusic(introPath, loopPath);
    Mix_PlayMusic(currentTrack.introTrack, 1);
    trackLoops = loops;
}

void KON_StopMusic() {
    Mix_HaltMusic();
}

void KON_MusicDaemon() {
    if (trackLoops == 0)
        return;
    if (KON_IsMusicPlaying() || !currentTrack.loopTrack)
        return;
    
    Mix_PlayMusic(currentTrack.loopTrack, trackLoops);
    trackLoops = 0;
}

bool KON_IsMusicPlaying() {
    return Mix_PlayingMusic();
}