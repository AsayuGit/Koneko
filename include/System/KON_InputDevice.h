#ifndef _KON_INPUTDEVICE_H
#define _KON_INPUTDEVICE_H

    #include <Bool.h>
    #include "KON_Events.h"

    typedef struct {
        /* Key Presses */
        KON_Event event;
        const uint8_t* KeyStates; /* Pointer to the keypresses */
    } InputDevice;

    typedef enum {
        KON_BINDING_GAMEPAD_BUTTON,
        KON_BINDING_GAMEPAD_AXIS,
        KON_BINDING_MOUSE_BUTTON,
        KON_BINDING_MOUSE_AXIS,
        KON_BINDING_KEY
    } KON_BindingType;

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

    /* UserActions */

    /*
        TODO: Doc
    */
    void KON_RegisterAction(unsigned int actionID, KON_BindingType type, unsigned int binding);

    /*
        TODO: Doc
    */
    void KON_DeleteAction(unsigned int actionID);

    /*
        TODO: Doc
    */
    void KON_AddActionBinding(unsigned int actionID, KON_BindingType type, unsigned int binding);

    /*
        SUMMARY : Test if the conditions for an Action are met
        INPUT   : unsigned int actionID : The Action to test for
        OUTPUT  : bool                  : Wether or not the conditions are met
    */
    bool KON_PollAction(unsigned int actionID);

#endif