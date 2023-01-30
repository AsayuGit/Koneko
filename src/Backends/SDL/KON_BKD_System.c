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

#include "KON_BKD_System.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "Log.h"

void KON_BKD_InitAudio(void) {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0){
        KON_SystemMsg("(KON_InitSoundDevice) Can't create sound device : ", MESSAGE_ERROR, 1, SDL_GetError());
    }
}

void KON_BKD_FreeAudio(void) {
    Mix_CloseAudio();
}

void KON_BKD_Init(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        KON_SystemMsg("(KON_InitEngine) SDL Initialisation failed : ", MESSAGE_ERROR, 1, SDL_GetError());
    }
}

void KON_BKD_Free(void) {
    SDL_Quit();
}

uint32_t KON_BKD_GetMs(void) {
    return SDL_GetTicks();
}