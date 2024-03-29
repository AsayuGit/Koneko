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
#include "KON_BKD_System.h"

#include <stdlib.h>

static void KON_FreeSoundDevice(void) {
    KON_BKD_FreeAudio();
}

static void KON_InitSoundDevice(void) {
    KON_BKD_InitAudio();
}

/* FIXME : Implement memoru cleanup */
void KON_Exit(void){
    KON_FreeFileSystem();
    KON_FreeInputDevice();
    KON_FreeSoundDevice();
    KON_FreeDisplayDevice();

    KON_BKD_Free();
}

void KON_InitEngine(int resX, int resY, char* gameTitle) {
    KON_BKD_Init();

    KON_InitDisplayDevice(resX, resY, gameTitle);
    KON_InitSoundDevice();
    KON_InitInputs();
    KON_InitFileSystem();
}

void KON_SystemEvents(void) {
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

uint32_t KON_GetMs(void) {
    return KON_BKD_GetMs();
}
