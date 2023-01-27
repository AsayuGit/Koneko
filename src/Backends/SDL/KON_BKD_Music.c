/*
    Koneko is a 2D/Raycast3D game engine written from scratch in ANSI C
    using SDL and libxml2. This engine is meant to allow game developpement
    for Linux, Windows and the og Xbox.

    Copyright (C) 2021-2023 Killian RAIMBAUD [Asayu] (killian.rai@gmail.com)

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

#include "KON_BKD_Music.h"

#include <SDL2/SDL_mixer.h>

KON_Track* KON_BKD_LoadTrack(char* path) {
    return Mix_LoadMUS(path);
}

void KON_BKD_FreeTrack(KON_Track* track) {
    Mix_FreeMusic(track);
}

void KON_BKD_PlayTrack(KON_Track* track, int loops) {
    Mix_PlayMusic(track, loops);
}

void KON_BKD_StopPlayback(void) {
    Mix_HaltMusic();
}

bool KON_BKD_GetPlaybackStatus(void) {
    return Mix_PlayingMusic();
}

void KON_BKD_SetPlaybackVolume(unsigned int volume) {
    Mix_VolumeMusic(volume);
}