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
#include "RessourceManager.h"
#include "Log.h"
#include "API.h"

#include <stddef.h>

#define KON_LoadRawMusic(FilePath) Mix_LoadMUS(FilePath)
#define KON_LoadRawSoundEffect(FilePath) Mix_LoadWAV(FilePath)

#define KON_FreeRawMusic(music) Mix_FreeMusic(music)
#define KON_FreeRawSoundEffect(soundEffect) Mix_FreeChunk(soundEffect)

KON_Music KON_LoadMusic(char* FilePath) {
    KON_Music loadingMusic;

    if (!FilePath)
        return NULL;

    if ((loadingMusic = KON_GetManagedRessource(FilePath, RESSOURCE_MUSIC)))
        return loadingMusic;

    if (!(loadingMusic = Mix_LoadMUS(FilePath))) {
        KON_SystemMsg("(KON_LoadMusic) Can't load music file : ", MESSAGE_ERROR, 2, FilePath, Mix_GetError());
        return NULL;
    }

    KON_AddManagedRessource(FilePath, RESSOURCE_MUSIC, loadingMusic);

    return loadingMusic;
}

KON_Sfx KON_LoadSoundEffect(char* FilePath) {
    KON_Sfx loadingSoundEffect;

    if (!FilePath)
        return NULL;

    if ((loadingSoundEffect = KON_GetManagedRessource(FilePath, RESSOURCE_SOUND_EFFECT)))
        return loadingSoundEffect;

    if (!(loadingSoundEffect = Mix_LoadWAV(FilePath))) {
        KON_SystemMsg("(KON_LoadSoundEffect) Can't load sound effect file : ", MESSAGE_ERROR, 2, FilePath, Mix_GetError());
        return NULL;
    }

    KON_AddManagedRessource(FilePath, RESSOURCE_SOUND_EFFECT, loadingSoundEffect);

    return loadingSoundEffect;
}

void KON_FreeMusic(KON_Music music) {
    if (!music)
        return;

    KON_FreeRawMusic(KON_FreeManagedRessourceByRef(music));
}

void KON_FreeSoundEffect(KON_Sfx soundEffect) {
    if (!soundEffect)
        return;

    KON_FreeRawSoundEffect(KON_FreeManagedRessourceByRef(soundEffect));
}

void KON_PlayMusic(KON_Music music, int loops) {
    Mix_PlayMusic(music, loops);
}

void KON_PlaySoundEffect(KON_Sfx sfx) {

}

void KON_StopMusic() {
    Mix_HaltMusic();
}

bool KON_IsMusicPlaying() {
    return Mix_PlayingMusic();
}