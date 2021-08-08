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

#ifndef _SYSTEM_H
#define _SYSTEM_H

    #include "include.h"

    enum {
        KON_INIT_AUDIO = SDL_INIT_AUDIO,
        KON_INIT_VIDEO = SDL_INIT_VIDEO,
        KON_INIT_INPUT = 0x40u,

        KON_INIT_EVERYTHING = 0x70u
    };

    void KON_Exit(KONDevice* KDevice);
    KONDevice* KON_Init(Uint32 flags, int resX, int resY, char* GameTitle);
    void FreeInputDevice(InputDevice* IDevice);
    void FreeDisplayDevice(DisplayDevice* DDevice);
    void FreeSoundDevice(void);
    int ACE_SetRenderTarget(DisplayDevice* DDevice, SDL_Texture* surface);
    int DrawEx(DisplayDevice* DDevice, SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect, bool flip);
    int Draw(DisplayDevice* DDevice, SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect);
    int ScaledDrawEx(DisplayDevice* DDevice, SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect, bool flip);
    int ScaledDraw(DisplayDevice* DDevice, SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect);
    void FinishFrame(DisplayDevice* DDevice);
    void SystemEvents(DisplayDevice* DDevice, InputDevice* IDevice);
    
#endif