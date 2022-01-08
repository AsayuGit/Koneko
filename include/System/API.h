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

#ifndef _API_H
#define _API_H

    #if defined(_XBOX)
        #define SDLMAIN <SDL.h>
        /*#define SDLIMAGE <SDL2/SDL_image.h>*/
        #define SDLMIXER <SDL_mixer.h>
    #else
        #define SDLMAIN <SDL2/SDL.h>
        #define SDLIMAGE <SDL2/SDL_image.h>
        #define SDLMIXER <SDL2/SDL_mixer.h>
    #endif

    #include SDLMAIN
    /*#include SDLIMAGE*/
    #include SDLMIXER

    #if defined(_XBOX)
        #define PADKEY jbutton.button
        #define PAD_KEYDOWN SDL_JOYBUTTONDOWN
    #else
        #define PADKEY key.keysym.scancode
        #define PAD_KEYDOWN SDL_KEYDOWN
    #endif

#endif