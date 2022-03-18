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

#ifndef _SYSTEM_H
#define _SYSTEM_H

    #include "Log.h"
    #include "KON_DisplayDevice.h"

    #include <Bool.h>

    enum {
        KON_INIT_AUDIO = SDL_INIT_AUDIO,
        KON_INIT_VIDEO = SDL_INIT_VIDEO,
        KON_INIT_INPUT = 0x40u,

        KON_INIT_EVERYTHING = 0x70u
    };

    typedef struct{
        /* Events */
        SDL_Event event;
        bool EventEnabled;

        /* Key Presses */
        const uint8_t* KeyStates; /* Pointer to the keypresses */
        
        /* Joystick */
        SDL_Joystick* Joy1; /* Pointers to the Joypad */
        bool JoyEnabled;
    } InputDevice;

    typedef struct {
        DisplayDevice dDevice;
        InputDevice iDevice;
    } KONDevice;

    /*
        SUMMARY : Properly exits Koneko.
    */
    void KON_Exit();

    /*
        SUMMARY : Initialize the Koneko Engine.
        INPUT   : uint32_t flags  : Any combination of initialisation flags.
        INPUT   : int resX, resY  : The screen resolution.
        INPUT   : char* gameTitle : The game's title.
    */
    void KON_Init(uint32_t flags, int resX, int resY, char* gameTitle);

    /*
        SUMMARY : Finalise the current frame and displays it in the game's window.
    */
    void KON_FinishFrame();

    /*
        SUMMARY : Process Koneko's system events.
    */
    void KON_SystemEvents();

    /*
        SUMMARY : Set if the engine should draw the performance header or not.
        INPUT   : bool value : true = on, false = off.
    */
    void KON_SetDrawFPS(bool value);

#endif