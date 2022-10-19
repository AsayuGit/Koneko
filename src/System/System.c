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

#include "Koneko.h"
#include "System.h"
#include "CommunFunctions.h"
#include "KeyMap.h"
#include "Graphics.h"
#include "KON_TextRendering.h"
#include "KON_FileSystem.h"

#include "API.h"
#include <stdlib.h>

static void KON_FreeSoundDevice() {
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        Mix_CloseAudio();
    #endif
}

static void KON_InitSoundDevice() {
    #ifdef GEKKO

    #else
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512) < 0){
            KON_SystemMsg("(KON_InitSoundDevice) Can't create sound device : ", MESSAGE_ERROR, 1, SDL_GetError());
        }
    #endif
}

/* FIXME : Implement memoru cleanup */
void KON_Exit(){
    KON_FreeFileSystem();
    KON_FreeInputDevice();
    KON_FreeSoundDevice();
    KON_FreeDisplayDevice();

    #ifndef GEKKO
        SDL_Quit();
    #endif
}

void KON_InitEngine(int resX, int resY, char* gameTitle) {
    #ifndef GEKKO
        /* SDL Init */
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            KON_SystemMsg("(KON_InitEngine) SDL Initialisation failed : ", MESSAGE_ERROR, 1, SDL_GetError());
        }
    #endif

    KON_InitDisplayDevice(resX, resY, gameTitle);
    KON_InitSoundDevice();
    KON_InitInputs();
    KON_InitFileSystem();
}

void KON_SystemEvents() {

    switch (Koneko.iDevice.event.type) {
        case KON_EVENT_GAME_EXIT:
            /* TODO: make it nicer */
            exit(0);
            break;

        case KON_EVENT_RESOLUTION_CHANGED:
            KON_UpdateRenderRect();
            break;

        case KON_EVENT_KEY_DOWN:

            /*

            switch (Koneko.iDevice.event.PADKEY)
            {
            case PAD_QUIT:
                exit(0); 
                break;
            
            case PAD_FULLSCREEN:
                SDL_SetWindowFullscreen(Koneko.dDevice.Screen, (SDL_GetWindowFlags(Koneko.dDevice.Screen) & SDL_WINDOW_FULLSCREEN_DESKTOP) ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
                break;

            case PAD_SNAP:
                SDL_SetWindowSize(Koneko.dDevice.Screen, Koneko.dDevice.RenderRect.w, Koneko.dDevice.RenderRect.h);
                KON_UpdateRenderRect();
                break;

            case PAD_INTEGER_SCLLING:
                Koneko.dDevice.integerScalling ^= 1;
                KON_UpdateRenderRect();
                break;

            default:
                break;
            }
            break;

            */

        default:
            break;
    }

}

uint32_t KON_GetMs() {
    #ifdef GEKKO
        /* TODO: implement libogc */
        return 0;
    #else
        return SDL_GetTicks();
    #endif
}