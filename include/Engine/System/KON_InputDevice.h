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

#ifndef _KON_INPUTDEVICE_H
#define _KON_INPUTDEVICE_H

    #include "Bool.h"
    #include "KON_Events.h"
    #include "KON_Keyboard.h"
    #include "KON_Mouse.h"
    #include "KON_Joy.h"

    typedef struct {
        /* Key Presses */
        KON_Event event;
        const uint8_t* KeyStates; /* Pointer to the keypresses */
        uint32_t mouseState;
        Vector2i mousePos;
        Vector2i mouseMvt;
    } InputDevice;

    /*
        TODO: Doc
    */
    void KON_InitInputs(void);

    /*
        TODO: Doc
    */
    void KON_FreeInputDevice(void);

    /*
        TODO: Doc
    */
    void KON_PumpEvent(void);

    /*
        TODO: Doc
    */
    bool KON_PollEvent(void);

    /* UserActions */

    /*
        TODO: Doc
    */
    void KON_RegisterAction(unsigned int actionID, KON_BindingType type, unsigned int binding, float weight);

    /*
        TODO: Doc
    */
    void KON_DeleteAction(unsigned int actionID);

    /*
        TODO: Doc
    */
    void KON_FreeUserActions(void);

    /*
        TODO: Doc
    */
    void KON_AddActionBinding(unsigned int actionID, KON_BindingType type, unsigned int binding, float weight);

    /*
        SUMMARY : Test if the conditions for an Action are met
        INPUT   : unsigned int actionID               : The Action to test for
        INPUT   : struct KON_EventAction* eventAction : Data relative to the action
        OUTPUT  : bool                                : Wether or not the conditions are met
    */
    bool KON_PollAction(unsigned int actionID, struct KON_EventAction* eventAction);

#endif
