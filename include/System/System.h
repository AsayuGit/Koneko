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

    #include "Log.h"
    
    #include "API.h" /* FIXME: Temporary */
    #include "Rect.h"

    #include <Bool.h>

    enum {
        KON_INIT_AUDIO = SDL_INIT_AUDIO,
        KON_INIT_VIDEO = SDL_INIT_VIDEO,
        KON_INIT_INPUT = 0x40u,

        KON_INIT_EVERYTHING = 0x70u
    };

     typedef struct{
        SDL_Window *Screen;
        #ifdef _SDL
            SDL_Surface* Renderer;
        #else
            SDL_Renderer *Renderer;
        #endif

        KON_Rect Frame[4];              /* Screen Border Frame */

        KON_Rect RenderRect;            /* Where the game is drawn on screen */
        Vector2i InternalResolution;    /* Internal render resolution of the game */
        Vector2i ScreenResolution;      /* The external resolution of the game */
        double IRScalar;                /* The scaling nessary match the external resolution */
        bool integerScalling;           /* If set to true then the output will be pixel perfect (provided the winow is big enough) */
        Vector2d Camera;                /* The coordinates of the in game camera */

        bool OffScreenRender;

        /* Timing control */
        Uint32 lastFrame;
        Uint32 frametime;
    } DisplayDevice;

    typedef struct{
        /* Events */
        SDL_Event event;
        bool EventEnabled;

        /* Key Presses */
        const Uint8* KeyStates; /* Pointer to the keypresses */
        
        /* Joystick */
        SDL_Joystick* Joy1; /* Pointers to the Joypad */
        bool JoyEnabled;
    } InputDevice;

    typedef struct {
        DisplayDevice* DDevice;
        InputDevice* IDevice;
    } KONDevice;

    void       KON_Exit(KONDevice* KDevice);
    KONDevice* KON_Init(Uint32 flags, int resX, int resY, char* GameTitle);
    void       KON_FreeInputDevice(InputDevice* IDevice);
    void       KON_FreeDisplayDevice(DisplayDevice* DDevice);
    void       KON_FreeSoundDevice(void);

    int        KON_SetRenderTarget(DisplayDevice* DDevice, SDL_Texture* surface);

    void       KON_FinishFrame(DisplayDevice* DDevice);
    void       KON_SystemEvents(DisplayDevice* DDevice, InputDevice* IDevice);

#endif