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
#include "KON_BKD_SFX.h"
#include "Log.h"
#include "RessourceManager.h"

#include <stdlib.h>

KON_SFX* KON_LoadSoundEffect(char* filePath) {
    KON_SFX* loadingSoundEffect;

    if (!filePath)
        return NULL;

    if ((loadingSoundEffect = (KON_SFX*)KON_GetManagedRessource(filePath, RESSOURCE_SOUND_EFFECT)))
        return loadingSoundEffect;

    if (!(loadingSoundEffect = KON_BKD_LoadSFX(filePath))) {
        KON_SystemMsg("(KON_LoadSoundEffect) Can't load sound effect file : ", MESSAGE_ERROR, 1, filePath);
        return NULL;
    }

    KON_AddManagedRessource(filePath, RESSOURCE_SOUND_EFFECT, loadingSoundEffect);

    return loadingSoundEffect;
}

void KON_FreeSoundEffect(KON_SFX* soundEffect) {
    if (!soundEffect)
        return;

    KON_BKD_FreeSFX((KON_SFX*)KON_FreeManagedRessourceByRef(soundEffect));
}

void KON_PlaySoundEffect(KON_SFX* sfx, int loops) {
    KON_BKD_PlaySFX(sfx, loops);
}

void KON_SetSoundEffectVolume(KON_SFX* sfx, unsigned int volume) {
    KON_BKD_SetSFXVolume(sfx, volume);
}