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

#include "System.h"
#include "CommunFunctions.h"
#include "KeyMap.h"
#include "Graphics.h"

void KON_UpdateRenderRect(DisplayDevice* DDevice){
    int ScreenWidth, ScreenHeight; /* Signed because SDL said so :c */

    #ifdef _SDL
        ScreenWidth = DDevice->Screen->w;
        ScreenHeight = DDevice->Screen->h;
    #else
        SDL_GetWindowSize(DDevice->Screen, &ScreenWidth, &ScreenHeight);
    #endif

    if (DDevice->integerScalling && (ScreenWidth > DDevice->InternalResolution.x) && (ScreenHeight > DDevice->InternalResolution.y))
        DDevice->IRScalar = MIN(ScreenWidth / DDevice->InternalResolution.x, ScreenHeight / DDevice->InternalResolution.y);
    else
        DDevice->IRScalar = MIN((double)ScreenWidth / (double)DDevice->InternalResolution.x, (double)ScreenHeight / (double)DDevice->InternalResolution.y);

    DDevice->ScreenResolution.x = ScreenWidth;
    DDevice->ScreenResolution.y = ScreenHeight;

    DDevice->RenderRect.w = DDevice->InternalResolution.x * DDevice->IRScalar;
    DDevice->RenderRect.h = DDevice->InternalResolution.y * DDevice->IRScalar;

    DDevice->RenderRect.x = (DDevice->ScreenResolution.x - DDevice->RenderRect.w) >> 1;
    DDevice->RenderRect.y = (DDevice->ScreenResolution.y - DDevice->RenderRect.h) >> 1;

    DDevice->OffScreenRender = false;

    DDevice->Frame[0] = KON_InitRect(0, 0, DDevice->RenderRect.x, ScreenHeight);                                                                                                            /* Left Frame */
    DDevice->Frame[1] = KON_InitRect(DDevice->RenderRect.x + DDevice->RenderRect.w, 0, DDevice->RenderRect.x, ScreenHeight);                                                /* Right Frame */
    DDevice->Frame[2] = KON_InitRect(DDevice->RenderRect.x, 0, DDevice->RenderRect.w, DDevice->RenderRect.y);                                                               /* Top Frame */
    DDevice->Frame[3] = KON_InitRect(DDevice->RenderRect.x, DDevice->RenderRect.y + DDevice->RenderRect.h, DDevice->RenderRect.w, DDevice->RenderRect.y);   /* Bottom Frame */
}

void KON_FreeDisplayDevice(DisplayDevice* DDevice){
    #ifdef _SDL
        SDL_FreeSurface(DDevice->Screen);
    #else
        SDL_DestroyRenderer(DDevice->Renderer);
        SDL_DestroyWindow(DDevice->Screen);
    #endif
    free(DDevice);
}

DisplayDevice* KON_CreateDisplayDevice(int resX, int resY, char* GameTitle){
    DisplayDevice* Device = (DisplayDevice*)calloc(1, sizeof(DisplayDevice));
    
    #ifdef _SDL
        Device->Screen = SDL_SetVideoMode(ScreenWidth, ScreenHeight, 32, SDL_HWSURFACE); /* | SDL_RESIZABLE */
        SDL_WM_SetCaption(GameTitle, NULL);
	    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1); /* VSync */
        Device->Renderer = Device->Screen;
    #else
        Device->Screen = SDL_CreateWindow(GameTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resX, resY, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    #endif
    if (Device->Screen == NULL){
        fprintf(stderr, "Can't create main window\n - %s\n", SDL_GetError());
        exit(-1);
    }
    #ifndef _SDL
        Device->Renderer = SDL_CreateRenderer(Device->Screen , -1, 0);
        if (Device->Renderer == NULL){
            fprintf(stderr, "Can't create main renderer\n - %s\n", SDL_GetError());
            exit(-1);
        }
        SDL_GL_SetSwapInterval(1); /* VSync */
    #endif

    /* TEMPORARY: Makes the start resulution de default internal resolution */
    Device->InternalResolution.x = resX;
    Device->InternalResolution.y = resY;

    KON_UpdateRenderRect(Device);

    return Device;
}

void KON_FreeSoundDevice(void){
    Mix_CloseAudio();
}

void KON_CreateSoundDevice(){
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512) < 0){
        fprintf(stderr, "Can't create sound device\n - %s\n", SDL_GetError());
        exit(-1);
    }
    /*return NULL;*/
}

void KON_FreeInputDevice(InputDevice* IDevice){
    if (IDevice->Joy1)
        SDL_JoystickClose(IDevice->Joy1);
    free(IDevice);
}

InputDevice* KON_InitInputs(void){
    InputDevice* Inputs;
    
    Inputs = (InputDevice*)calloc(1, sizeof(InputDevice));
    Inputs->Joy1 = NULL;
    if (SDL_NumJoysticks()){
        Inputs->Joy1 = SDL_JoystickOpen(0); /* Open Joystick */
    }
    Inputs->KeyStates = SDL_GetKeyboardState(NULL); /* Open Keyboard */
    if (Inputs->Joy1 == NULL){
        Inputs->JoyEnabled = false;
    }else{
        Inputs->JoyEnabled = true;
    }
    Inputs->EventEnabled = true;
    return Inputs;
}

void KON_Exit(KONDevice* KDevice){

}

KONDevice* KON_Init(Uint32 flags, int resX, int resY, char* GameTitle){
    KONDevice* KDevice = NULL;

    KDevice = (KONDevice*)calloc(1, sizeof(KONDevice));

    /* SDL Init */
    if (SDL_Init(flags) != 0){
        fprintf(stderr, "[KON] SDL Initialisation failed\n - %s\n", SDL_GetError());
        exit(-1);
    }

    if (flags & KON_INIT_VIDEO){
        KDevice->DDevice = KON_CreateDisplayDevice(resX, resY, GameTitle);
    }
    if (flags & KON_INIT_AUDIO){
        KON_CreateSoundDevice();
    }
    if (flags & KON_INIT_INPUT){
        KDevice->IDevice = KON_InitInputs();
    }

    return KDevice;
}

int KON_SetRenderTarget(DisplayDevice* DDevice, SDL_Texture* surface){
    DDevice->OffScreenRender = (surface != NULL);
#ifdef _SDL
    DDevice->Renderer = (surface) ? surface : DDevice->Screen;
    return 0;
#else
    return SDL_SetRenderTarget(DDevice->Renderer, surface);
#endif
}

int KON_DrawEx(DisplayDevice* DDevice, SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect, bool flip){
	#ifdef _SDL
        FlipBlitSurface(texture, srcrect, DDevice->Renderer, dstrect, flip);
        return 0;
    #else
        return SDL_RenderCopyEx(DDevice->Renderer, texture, srcrect, dstrect, 0, 0, flip);
    #endif
}

int KON_Draw(DisplayDevice* DDevice, SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect){
    return KON_DrawEx(DDevice, texture, srcrect, dstrect, 0);
}

int KON_ScaledDrawEx(DisplayDevice* DDevice, SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect, bool flip){
    SDL_Rect ScaledDstRect;
    #ifdef _SDL
        SDL_Rect ScaledSrcRect;
    #endif

    ScaledDstRect = KON_InitRect(0, 0, DDevice->InternalResolution.x, DDevice->InternalResolution.y);
    if (texture && RectOnScreen(DDevice, dstrect)){
        
        #ifdef _SDL
            if (srcrect){
                ScaledSrcRect = KON_InitRect(
                    (srcrect->x * DDevice->IRScalar) + DDevice->RenderRect.x,
                    (srcrect->y * DDevice->IRScalar) + DDevice->RenderRect.y,
                    srcrect->w * DDevice->IRScalar,
                    srcrect->h * DDevice->IRScalar
                );
            }
        #endif

        if (dstrect){
            ScaledDstRect = KON_InitRect(
                (dstrect->x * DDevice->IRScalar) + DDevice->RenderRect.x,
                (dstrect->y * DDevice->IRScalar) + DDevice->RenderRect.y,
                dstrect->w * DDevice->IRScalar,
                dstrect->h * DDevice->IRScalar
            );
        } else {
            ScaledDstRect = DDevice->RenderRect;
        }
        #ifdef _SDL
            return KON_DrawEx(DDevice, texture, &ScaledSrcRect, &ScaledDstRect, flip);
        #else
            return KON_DrawEx(DDevice, texture, srcrect, &ScaledDstRect, flip);
        #endif
    }
    return 0;
}

int KON_ScaledDraw(DisplayDevice* DDevice, SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect){
    return KON_ScaledDrawEx(DDevice, texture, srcrect, dstrect, false);
}

void KON_FinishFrame(DisplayDevice* DDevice){
    Uint32 ticks;
    DrawFrame(DDevice);
    #ifdef _SDL
        SDL_Flip(DDevice->Screen);
    #else
        SDL_RenderPresent(DDevice->Renderer);
    #endif
    ticks = SDL_GetTicks();
    DDevice->frametime = ticks - DDevice->lastFrame;
    DDevice->lastFrame = ticks;
}

void KON_SystemEvents(DisplayDevice* DDevice, InputDevice* IDevice){
	SDL_Event assertedEvent;

    switch (IDevice->event.type){
        #ifndef _SDL
        case SDL_WINDOWEVENT:
            switch (IDevice->event.window.event)
            {
            case SDL_WINDOWEVENT_RESIZED:
                KON_UpdateRenderRect(DDevice);
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
			switch (IDevice->event.jhat.value){
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
            switch (IDevice->event.PADKEY)
            {
            case PAD_QUIT:
                exit(0); /* FIXME: We need a cleaner way of exiting out of the game */
                break;
            
            case PAD_FULLSCREEN:
                SDL_SetWindowFullscreen(DDevice->Screen, (SDL_GetWindowFlags(DDevice->Screen) & SDL_WINDOW_FULLSCREEN_DESKTOP) ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
                break;

            case PAD_SNAP:
                SDL_SetWindowSize(DDevice->Screen, DDevice->RenderRect.w, DDevice->RenderRect.h);
                KON_UpdateRenderRect(DDevice);
                break;

            case PAD_INTEGER_SCLLING:
                DDevice->integerScalling ^= 1;
                KON_UpdateRenderRect(DDevice);
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