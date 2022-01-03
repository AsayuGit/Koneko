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

#ifndef _TYPES_H
#define _TYPES_H

    #ifdef _SDL
        #define SDLMAIN <SDL/SDL.h>
        #define SDLIMAGE <SDL/SDL_image.h>
        #define SDLMIXER <SDL/SDL_mixer.h>
    #elif defined(_XBOX)
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
	
	#ifdef _XBOX
		typedef enum{
			false,
			true
		} bool;
	#else
		#include <stdbool.h>
	#endif

    #ifdef _SDL
        #include <SDL/SDL.h>
    #elif defined(_XBOX)
		#include <SDL.h>
	#else
        #include <SDL2/SDL.h>
    #endif

    typedef void (*functPtr)(void);

    /* Uniformity between SDL1.2 and SDL2 */
    #ifdef _SDL
        typedef SDL_Surface SDL_Window;
        typedef SDL_Surface SDL_Texture;
        #define SDL_GetKeyboardState SDL_GetKeyState
        #define PADKEY key.keysym.sym
		#define PAD_KEYDOWN SDL_KEYDOWN
	#elif defined(_XBOX)
		#define SDL_HWSURFACE SDL_SWSURFACE
        #define PADKEY jbutton.button
		#define PAD_KEYDOWN SDL_JOYBUTTONDOWN
    #else
        #define SDL_HWSURFACE SDL_SWSURFACE
        #define PADKEY key.keysym.scancode
		#define PAD_KEYDOWN SDL_KEYDOWN
    #endif

    typedef struct{
        int x;
        int y;
    } Vector2i;

    typedef struct{
        double x;
        double y;
    } Vector2d;

    typedef struct{
        SDL_Window *Screen;
        #ifdef _SDL
            SDL_Surface* Renderer;
        #else
            SDL_Renderer *Renderer;
        #endif

        SDL_Rect Frame[4];              /* Screen Border Frame */

        SDL_Rect RenderRect;            /* Where the game is drawn on screen */
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

    typedef enum {
        Right,
        DownRight,
        Down,
        DownLeft,
        Left,
        UpLeft,
        Up,
        UpRight
    } Direction;

    SDL_Rect     KON_InitRect(int x, int y, int w, int h);
    Vector2d     KON_InitVector2d(double x, double y);
    Vector2i     KON_InitVector2i(int x, int y);
    SDL_Rect     KON_RectPlusVector2i(SDL_Rect* Rect, Vector2i* Vect); /* FIXME: present in CommunFunctions (Vector2d) */
    SDL_Rect     KON_RectMinusVector2i(SDL_Rect* Rect, Vector2i* Vect);

#endif