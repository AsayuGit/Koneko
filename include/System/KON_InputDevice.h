#ifndef _KON_INPUTDEVICE_H
#define _KON_INPUTDEVICE_H

    #include <stdint.h>
    #include <Bool.h>

    enum {
        KON_EVENT_NONE = 0,
        KON_GAME_EXIT = 1,
        KON_RESOLUTION_CHANGED = 2,
        KON_KEY_DOWN = 4,
        KON_KEY_UP = 8
    };

    struct KON_EventResChanged {
        uint8_t type;
        unsigned int width;
        unsigned int height;
    };

    typedef union {
        uint8_t type;
        struct KON_EventResChanged res;
    } KON_Event;

    typedef struct{
        /* Key Presses */
        KON_Event event;
        const uint8_t* KeyStates; /* Pointer to the keypresses */
    } InputDevice;

    /*
        TODO: Doc
    */
    void KON_InitInputs();

    /*
        TODO: Doc
    */
    void KON_FreeInputDevice();

    /*
        TODO: Doc
    */
    bool KON_PollEvent();

#endif