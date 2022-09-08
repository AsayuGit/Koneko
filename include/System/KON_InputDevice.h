#ifndef _KON_INPUTDEVICE_H
#define _KON_INPUTDEVICE_H

    #include <Bool.h>
    #include "KON_Events.h"

    typedef struct {
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