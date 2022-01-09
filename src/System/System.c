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
#include "CommunFunctions.h"
#include "KeyMap.h"
#include "Graphics.h"

void KON_UpdateRenderRect(){
    int ScreenWidth, ScreenHeight; /* Signed because SDL said so :c */

    SDL_GetWindowSize(Koneko.dDevice.Screen, &ScreenWidth, &ScreenHeight);

    if (Koneko.dDevice.integerScalling && (ScreenWidth > Koneko.dDevice.InternalResolution.x) && (ScreenHeight > Koneko.dDevice.InternalResolution.y))
        Koneko.dDevice.IRScalar = MIN(ScreenWidth / Koneko.dDevice.InternalResolution.x, ScreenHeight / Koneko.dDevice.InternalResolution.y);
    else
        Koneko.dDevice.IRScalar = MIN((double)ScreenWidth / (double)Koneko.dDevice.InternalResolution.x, (double)ScreenHeight / (double)Koneko.dDevice.InternalResolution.y);

    Koneko.dDevice.ScreenResolution.x = ScreenWidth;
    Koneko.dDevice.ScreenResolution.y = ScreenHeight;

    Koneko.dDevice.RenderRect.w = Koneko.dDevice.InternalResolution.x * Koneko.dDevice.IRScalar;
    Koneko.dDevice.RenderRect.h = Koneko.dDevice.InternalResolution.y * Koneko.dDevice.IRScalar;

    Koneko.dDevice.RenderRect.x = (int)(Koneko.dDevice.ScreenResolution.x - Koneko.dDevice.RenderRect.w) >> 1;
    Koneko.dDevice.RenderRect.y = (int)(Koneko.dDevice.ScreenResolution.y - Koneko.dDevice.RenderRect.h) >> 1;

    Koneko.dDevice.OffScreenRender = false;

    KON_InitRect(Koneko.dDevice.Frame[0], 0, 0, Koneko.dDevice.RenderRect.x, ScreenHeight);                                                                                                            /* Left Frame */
    KON_InitRect(Koneko.dDevice.Frame[1], Koneko.dDevice.RenderRect.x + Koneko.dDevice.RenderRect.w, 0, Koneko.dDevice.RenderRect.x, ScreenHeight);                                                /* Right Frame */
    KON_InitRect(Koneko.dDevice.Frame[2], Koneko.dDevice.RenderRect.x, 0, Koneko.dDevice.RenderRect.w, Koneko.dDevice.RenderRect.y);                                                               /* Top Frame */
    KON_InitRect(Koneko.dDevice.Frame[3], Koneko.dDevice.RenderRect.x, Koneko.dDevice.RenderRect.y + Koneko.dDevice.RenderRect.h, Koneko.dDevice.RenderRect.w, Koneko.dDevice.RenderRect.y);   /* Bottom Frame */
}

static void KON_FreeDisplayDevice() {
    SDL_DestroyRenderer(Koneko.dDevice.Renderer);
    SDL_DestroyWindow(Koneko.dDevice.Screen);
}

void KON_CreateDisplayDevice(int resX, int resY, char* GameTitle) {
    Koneko.dDevice.Screen = SDL_CreateWindow(GameTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resX, resY, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    Koneko.dDevice.Renderer = SDL_CreateRenderer(Koneko.dDevice.Screen , -1, 0);
    if (Koneko.dDevice.Renderer == NULL){
        KON_SystemMsg("(KON_CreateDisplayDevice) Can't create main renderer : ", MESSAGE_ERROR, 1, SDL_GetError());
    }
    SDL_GL_SetSwapInterval(true); /* VSync */

    /* FIXME : TEMPORARY: Makes the start resulution de default internal resolution */
    Koneko.dDevice.InternalResolution.x = resX;
    Koneko.dDevice.InternalResolution.y = resY;

    KON_UpdateRenderRect();
}

static void KON_FreeSoundDevice() {
    Mix_CloseAudio();
}

static void KON_CreateSoundDevice() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512) < 0){
        KON_SystemMsg("(KON_CreateSoundDevice) Can't create sound device : ", MESSAGE_ERROR, 1, SDL_GetError());
    }
}

static void KON_FreeInputDevice() {
    if (Koneko.iDevice.Joy1)
        SDL_JoystickClose(Koneko.iDevice.Joy1);
}

void KON_InitInputs(){
    Koneko.iDevice.Joy1 = NULL;
    if (SDL_NumJoysticks())
        Koneko.iDevice.Joy1 = SDL_JoystickOpen(0); /* Open Joystick */
    
    Koneko.iDevice.KeyStates = SDL_GetKeyboardState(NULL); /* Open Keyboard */
    Koneko.iDevice.JoyEnabled = (Koneko.iDevice.Joy1);
    Koneko.iDevice.EventEnabled = true;
}

/* FIXME : Implement memoru cleanup */
void KON_Exit(){
    KON_FreeInputDevice();
    KON_FreeSoundDevice();
    KON_FreeDisplayDevice();
}

void KON_Init(uint32_t flags, int resX, int resY, char* GameTitle){
    /* SDL Init */
    if (SDL_Init(flags) != 0){
        KON_SystemMsg("(KON_Init) SDL Initialisation failed : ", MESSAGE_ERROR, 1, SDL_GetError());
    }

    if (flags & KON_INIT_VIDEO)
        KON_CreateDisplayDevice(resX, resY, GameTitle);
    if (flags & KON_INIT_AUDIO)
        KON_CreateSoundDevice();
    if (flags & KON_INIT_INPUT)
        KON_InitInputs();
}

int KON_SetRenderTarget(SDL_Texture* surface){
    Koneko.dDevice.OffScreenRender = (surface);
    return SDL_SetRenderTarget(Koneko.dDevice.Renderer, surface);
}

static void DrawFrame(){
    SDL_RenderFillRect(Koneko.dDevice.Renderer, (SDL_Rect*)&Koneko.dDevice.Frame[0]);
    SDL_RenderFillRect(Koneko.dDevice.Renderer, (SDL_Rect*)&Koneko.dDevice.Frame[1]);
    SDL_RenderFillRect(Koneko.dDevice.Renderer, (SDL_Rect*)&Koneko.dDevice.Frame[2]);
    SDL_RenderFillRect(Koneko.dDevice.Renderer, (SDL_Rect*)&Koneko.dDevice.Frame[3]);
}

void KON_FinishFrame(){
    Uint32 ticks;
    DrawFrame();
    SDL_RenderPresent(Koneko.dDevice.Renderer);
    ticks = SDL_GetTicks();
    Koneko.dDevice.frametime = ticks - Koneko.dDevice.lastFrame;
    Koneko.dDevice.lastFrame = ticks;
}

void KON_SystemEvents(){
    SDL_Event assertedEvent;

    switch (Koneko.iDevice.event.type){
        #ifndef _SDL
        case SDL_WINDOWEVENT:
            switch (Koneko.iDevice.event.window.event)
            {
            case SDL_WINDOWEVENT_RESIZED:
                KON_UpdateRenderRect();
                break;

            case SDL_WINDOWEVENT_CLOSE:
                exit(0);
                break;
            
            default:
                break;
            }
            break;

        case SDL_JOYHATMOTION:
            assertedEvent.type = PAD_KEYDOWN;
            switch (Koneko.iDevice.event.jhat.value){
                case SDL_HAT_LEFT:
                    assertedEvent.PADKEY = PAD_LEFT;
                    SDL_PushEvent(&assertedEvent);
                    break;
                case SDL_HAT_RIGHT:
                    assertedEvent.PADKEY = PAD_RIGHT;
                    SDL_PushEvent(&assertedEvent);
                    break;
                case SDL_HAT_UP:
                    assertedEvent.PADKEY = PAD_UP;
                    SDL_PushEvent(&assertedEvent);
                    break;
                case SDL_HAT_DOWN:
                    assertedEvent.PADKEY = PAD_DOWN;
                    SDL_PushEvent(&assertedEvent);
                    break;
                default:
                    break;
            }
            break;

        case SDL_KEYDOWN:
            switch (Koneko.iDevice.event.PADKEY)
            {
            case PAD_QUIT:
                exit(0); /* FIXME: We need a cleaner way of exiting out of the game */
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

        #endif

        default:
            break;
    }

}