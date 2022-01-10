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

#ifndef _JUKEBOX_H
#define _JUKEBOX_H

    /*
        SUMMARY : Plays a looping music file from disk.
        INPUT   : char* IntroPath : Path to the intro part of the music.
        INPUT   : char* LoopPath  : Path to the looping part of the music.
    */
    void KON_PlayTrackFromDisk(char* IntroPath, char* LoopPath);

    /*
        SUMMARY : Process the transition between the into part and the looping part of background tracks.
    */
    void KON_MusicDaemon();

    /*
        SUMMARY : Stops any previously started music track.
    */
    void KON_StopTrack();

#endif
