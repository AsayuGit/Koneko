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

#ifndef KON_EVENTS_H
#define KON_EVENTS_H

    #include <stdint.h>
    #include <Vector.h>

    typedef enum{
        KON_INPUT_IDLE,
        KON_INPUT_PRESSED,
        KON_INPUT_HELD,
        KON_INPUT_RELEASED
    } KON_InputState;

    typedef enum {
        KON_BINDING_GAMEPAD_BUTTON,
        KON_BINDING_GAMEPAD_AXIS,
        KON_BINDING_MOUSE_BUTTON,
        KON_BINDING_MOUSE_AXIS,
        KON_BINDING_KEY
    } KON_BindingType;

    enum {
        KON_EVENT_NONE = 0,
        KON_EVENT_GAME_EXIT,
        KON_EVENT_RESOLUTION_CHANGED,
        KON_EVENT_KEY_DOWN,
        KON_EVENT_KEY_UP,
        KON_EVENT_ACTION
    };

    struct KON_EventResChanged {
        uint8_t type;
        unsigned int width;
        unsigned int height;
    };

    struct KON_EventJoystick {
        uint8_t type;
        int8_t X;
        int8_t Y;
    };

    typedef union {
        struct KON_MouseActionData {
            Vector2i pos;
            Vector2i mvt;
        } mouse;
    } KON_ActionData;

    struct KON_EventAction {
        uint8_t type;
        unsigned int actionID;
        KON_InputState state;
        KON_BindingType bindingType;
        KON_ActionData data;
    };

    typedef union {
        uint8_t type;
        struct KON_EventResChanged res;
        struct KON_EventJoystick joy;
        struct KON_EventAction action;
    } KON_Event;

#endif
