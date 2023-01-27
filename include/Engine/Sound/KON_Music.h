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

#ifndef _MUSIC_H
#define _MUSIC_H

    #include "Bool.h"
    #include "KON_BKD_Music.h"

    /*
        SUMMARY : Play the music file passed in parameters
        INPUT   : char* introPath : The path of the music intro
        INPUT   : char* loopPath  : The path of the music loop
        INPUT   : int  loops       : The number of times to play the music for (-1 = infinity)
    */
    void KON_PlayMusic(char* introPath, char* loopPath, int loops);

    /*
        SUMMARY : Stop the currently playing music file if any
    */
    void KON_StopMusic(void);

    /*
        SUMMARY : Play the looping part of the currently playing music one the intro has ended (should be called every frame)
    */
    void KON_MusicDaemon(void);

    /*
        SUMMARY : Return wether music is currently playing or not.
    */
    bool KON_IsMusicPlaying(void);

    /*
        SUMMARY : Set the volume of the music track.
        INPUT   : unsigned int volume : The new volume the music should be played at.
    */
    void KON_SetMusicVolume(unsigned int volume);

#endif
