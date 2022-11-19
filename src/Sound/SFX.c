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

#include "SFX.h"
#include "API.h"
#include "Log.h"
#include "RessourceManager.h"

#include <stdlib.h>

#ifdef GEKKO
    typedef void KON_RawSfx;
#else
    typedef Mix_Chunk KON_RawSfx;
#endif

struct KON_Sfx {
    KON_RawSfx* sfx;
};

KON_RawSfx* KON_LoadRawSoundEffect(char* FilePath) {
    #ifdef GEKKO
        return NULL;
    #else
        return Mix_LoadWAV(FilePath);
    #endif
}

void KON_FreeRawSoundEffect(KON_RawSfx* soundEffect) {
    #ifndef GEKKO
        Mix_FreeChunk(soundEffect);
    #endif
}

void KON_PlayRawSoundEffect(KON_RawSfx* soundEffect, int loops) {
    #ifndef GEKKO
        Mix_PlayChannel(-1, soundEffect, loops);
    #endif
}

KON_Sfx* KON_LoadSoundEffect(char* filePath) {
    KON_Sfx* loadingSoundEffect;

    if (!filePath)
        return NULL;

    if ((loadingSoundEffect = (KON_Sfx*)KON_GetManagedRessource(filePath, RESSOURCE_SOUND_EFFECT)))
        return loadingSoundEffect;

    if (!(loadingSoundEffect = (KON_Sfx*)malloc(sizeof(KON_Sfx)))) {
        KON_SystemMsg("(KON_LoadSoundEffect) Couldn't allocate memory !", MESSAGE_ERROR, 0);
        return NULL;
    }

    if (!(loadingSoundEffect->sfx = KON_LoadRawSoundEffect(filePath))) {
        #ifdef GEKKO
            /* TODO: implement libogc */
        #else
            KON_SystemMsg("(KON_LoadSoundEffect) Can't load sound effect file : ", MESSAGE_ERROR, 2, filePath, Mix_GetError());
        #endif
        return NULL;
    }

    KON_AddManagedRessource(filePath, RESSOURCE_SOUND_EFFECT, loadingSoundEffect);

    return loadingSoundEffect;
}

void KON_FreeSoundEffect(KON_Sfx* soundEffect) {
    if (!soundEffect)
        return;

    KON_FreeRawSoundEffect((KON_RawSfx*)KON_FreeManagedRessourceByRef(soundEffect->sfx));
}

void KON_PlaySoundEffect(KON_Sfx* sfx, int loops) {
    KON_PlayRawSoundEffect(sfx->sfx, loops);
}

void KON_SetSoundEffectVolume(KON_Sfx* sfx, unsigned int volume) {
    #ifndef GEKKO
        Mix_VolumeChunk(sfx->sfx, volume);
    #endif
}
