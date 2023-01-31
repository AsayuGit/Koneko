/*
    Koneko is a 2D/Raycast3D game engine written from scratch in ANSI C
    using SDL and libxml2. This engine is meant to allow game developpement
    for Linux, Windows and the og Xbox.

    Copyright (C) 2021-2023 Killian RAIMBAUD [Asayu] (killian.rai@gmail.com)

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

#include "KON_BKD_Input.h"

#include <SDL2/SDL.h>
#include <stdint.h>

#include "Koneko.h"
#include "Bool.h"
#include "KON_Events.h"

typedef struct {
    /* Events */
    bool EventEnabled;

    /* Joystick */
    SDL_Joystick* Joy1; /* Pointers to the Joypad */
    bool JoyEnabled;
} KON_InputInterface;

static KON_InputInterface inInt;


void KON_BKD_InitInputs(void) {
    inInt.Joy1 = NULL;
    if (SDL_NumJoysticks()) inInt.Joy1 = SDL_JoystickOpen(0); /* Open Joystick */
    
    Koneko.iDevice.KeyStates = SDL_GetKeyboardState(NULL); /* Open Keyboard */
    inInt.JoyEnabled = inInt.Joy1 != 0;
    inInt.EventEnabled = true;
}

void KON_BKD_FreeInputs(void) {
    if (inInt.Joy1)
        SDL_JoystickClose(inInt.Joy1);
}

void KON_BKD_PumpSystemEvents(KON_FIFO* eventQueue) {
    SDL_Event sdlEvent;

     while (SDL_PollEvent(&sdlEvent)) {
        KON_Event newEvent = {0};

        switch (sdlEvent.type) {
        case SDL_WINDOWEVENT:
            switch (sdlEvent.window.event) {
                case SDL_WINDOWEVENT_CLOSE:
                    newEvent.type = KON_EVENT_GAME_EXIT;
                    break;

                case SDL_WINDOWEVENT_RESIZED:
                    newEvent.type = KON_EVENT_RESOLUTION_CHANGED;
                    newEvent.res.width = sdlEvent.window.data1;
                    newEvent.res.height = sdlEvent.window.data2;
                    break;
            }
            break;
        default:
            continue;
        }

        KON_FIFOPush(eventQueue, &newEvent);
    }
}

void KON_BKD_GetMouseState(void) {
    Koneko.iDevice.mouseState = SDL_GetMouseState(&Koneko.iDevice.mousePos.x, &Koneko.iDevice.mousePos.y);
    SDL_GetRelativeMouseState(&Koneko.iDevice.mouseMvt.x, &Koneko.iDevice.mouseMvt.y);
}

float KON_BKD_GetJoyAxisState(unsigned int axis, uint8_t player) {
    return (float)SDL_JoystickGetAxis(inInt.Joy1, axis) / INT16_MAX;
}