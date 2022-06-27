#include "KON_InputDevice.h"

#include "Koneko.h"
#include "API.h"

#include <stdbool.h>

typedef struct {
    #ifdef GEKKO

    #else
        /* Events */
        SDL_Event event;
        bool EventEnabled;

        /* Joystick */
        SDL_Joystick* Joy1; /* Pointers to the Joypad */
        bool JoyEnabled;
    #endif
} KON_InputInterface;

static KON_InputInterface inInt;

void KON_InitInputs() {
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        inInt.Joy1 = NULL;
        if (SDL_NumJoysticks())
        inInt.Joy1 = SDL_JoystickOpen(0); /* Open Joystick */
        
        Koneko.iDevice.KeyStates = SDL_GetKeyboardState(NULL); /* Open Keyboard */
        inInt.JoyEnabled = (bool)(inInt.Joy1);
        inInt.EventEnabled = true;
    #endif
}

void KON_FreeInputDevice() {
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        if (inInt.Joy1)
            SDL_JoystickClose(inInt.Joy1);
    #endif
}

bool KON_PollEvent() {
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        while (SDL_PollEvent(&inInt.event)) {
            switch (inInt.event.type) {
                case SDL_WINDOWEVENT:
                    switch (inInt.event.window.event) {
                        case SDL_WINDOWEVENT_CLOSE:
                            Koneko.iDevice.event.type = KON_GAME_EXIT;
                            return true;

                        case SDL_WINDOWEVENT_RESIZED:
                            Koneko.iDevice.event.type = KON_RESOLUTION_CHANGED;
                            Koneko.iDevice.event.res.width = inInt.event.window.data1;
                            Koneko.iDevice.event.res.height = inInt.event.window.data2;
                            return true;
                    }
                    break;

                case SDL_KEYDOWN:
                    Koneko.iDevice.event.type = KON_KEY_DOWN;
                    /* TODO Which key */
                    return true;

                case SDL_KEYUP:
                    Koneko.iDevice.event.type = KON_KEY_UP;
                    /* TODO Which key */
                    return true;
                
                default:
                    break;
            }
        }

        Koneko.iDevice.event.type = KON_EVENT_NONE;
        return false;
    #endif
}