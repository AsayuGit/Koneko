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

#ifndef _MUSIC_H
#define _MUSIC_H

    #include "Types.h"

    /*
        SUMARRY : Load a music file from disk.
        INPUT   : char* FilePath : The path to the music file.
        OUTPUT  : Mix_Music*     : The newly loaded music file or NULL on error.
    */
    Mix_Music* KON_LoadMusic(char* FilePath);

    /*
        SUMMARY : Load a sound effect file from disk.
        INPUT   : char* FilePath : The path to the sound effect file.
        OUTPUT  : Mix_Chunk*     : The newly loaded sound effect or NULL on error.
    */
    Mix_Chunk* KON_LoadSoundEffect(char* FilePath);

    /*
        SUMMARY : Free a previously loaded music file.
        INPUT   : Mix_Music* music : The music file to free.
    */
    void KON_FreeMusic(Mix_Music* music);
    
    /*
        SUMMARY : Free a previously loaded sound effect.
        INPUT   : Mix_Chunk* soundEffect : The sound effect to free.
    */
    void KON_FreeSoundEffect(Mix_Chunk* soundEffect);

#endif