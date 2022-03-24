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

extern struct BITMAP_SystemFont { int width; int height; int depth; int pitch; uint8_t pixels[113772];} SystemFont;

static bool drawFPS;
static BitmapFont* font;

SDL_Surface* testSurface;
SDL_Texture* testTexture;

void KON_SetDrawFPS(bool value) {
    drawFPS = value;
}

void KON_DrawFPS() {
    char fpsText[100];
    double fps = 1000.0 / Koneko.dDevice.frametime;

    sprintf(fpsText, "FPS: %.2f\nFrametime: %u ms", fps, Koneko.dDevice.frametime);
    gprintf(font, fpsText, 1, NULL);
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
    Koneko.iDevice.JoyEnabled = (bool)(Koneko.iDevice.Joy1);
    Koneko.iDevice.EventEnabled = true;
}

/* FIXME : Implement memoru cleanup */
void KON_Exit(){
    KON_FreeInputDevice();
    KON_FreeSoundDevice();
    KON_FreeDisplayDevice();
}

void KON_Init(uint32_t flags, int resX, int resY, char* gameTitle){
    BITMAP SystemFontBitmap;

    initBitmap(SystemFontBitmap, SystemFont);

    /* SDL Init */
    if (SDL_Init(flags) != 0){
        KON_SystemMsg("(KON_Init) SDL Initialisation failed : ", MESSAGE_ERROR, 1, SDL_GetError());
    }

    if (flags & KON_INIT_VIDEO)
        KON_CreateDisplayDevice(resX, resY, gameTitle);
    if (flags & KON_INIT_AUDIO)
        KON_CreateSoundDevice();
    if (flags & KON_INIT_INPUT)
        KON_InitInputs();

    font = KON_LoadBitmapFontFromMem(&SystemFontBitmap, 0xff00ff);
}

static void DrawFrame() {
    SDL_RenderFillRect(Koneko.dDevice.Renderer, (SDL_Rect*)&Koneko.dDevice.Frame[0]);
    SDL_RenderFillRect(Koneko.dDevice.Renderer, (SDL_Rect*)&Koneko.dDevice.Frame[1]);
    SDL_RenderFillRect(Koneko.dDevice.Renderer, (SDL_Rect*)&Koneko.dDevice.Frame[2]);
    SDL_RenderFillRect(Koneko.dDevice.Renderer, (SDL_Rect*)&Koneko.dDevice.Frame[3]);
}

void KON_FinishFrame(){
    Uint32 ticks;
    DrawFrame();

    if (drawFPS)
        KON_DrawFPS();

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