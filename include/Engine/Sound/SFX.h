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

#ifndef _SFX_H
#define _SFX_H

    #include "KON_BKD_SFX.h"

    /*
        SUMMARY : Load a sound effect file from disk.
        INPUT   : char* filePath : The path to the sound effect file.
        OUTPUT  : KON_SFX*       : The newly loaded sound effect or NULL on error.
    */
    KON_SFX* KON_LoadSoundEffect(char* filePath);


    /*
        SUMMARY : Free a previously loaded sound effect.
        INPUT   : KON_SFX soundEffect : The sound effect to free.
    */
    void KON_FreeSoundEffect(KON_SFX* soundEffect);

    /*
        SUMMARY : Plays the sound effect file passed in parameters
        INPUT   : KON_SFX sfx : The sound effect to play
        INPUT   : int loops   : The number of times the sound effect should be played for
    */
    void KON_PlaySoundEffect(KON_SFX* sfx, int loops);


    /*
        SUMMARY : Set the volume for a sound effect
        INPUT   : KON_SFX* sfx        : The sound effect to set the volume if
        INPUT   : unsigned int volume : The new volume value
    */
    void KON_SetSoundEffectVolume(KON_SFX* sfx, unsigned int volume);

#endif
