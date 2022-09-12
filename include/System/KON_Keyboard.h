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

#ifndef _KON_KEYBOARD_H
#define _KON_KEYBOARD_H

    /* Values from: https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf */
    typedef enum {
        KON_KEYBOARD_A = 0x04,
        KON_KEYBOARD_B,
        KON_KEYBOARD_C,
        KON_KEYBOARD_D,
        KON_KEYBOARD_E,
        KON_KEYBOARD_F,
        KON_KEYBOARD_G,
        KON_KEYBOARD_H,
        KON_KEYBOARD_I,
        KON_KEYBOARD_J,
        KON_KEYBOARD_K,
        KON_KEYBOARD_L,
        KON_KEYBOARD_M,
        KON_KEYBOARD_N,
        KON_KEYBOARD_O,
        KON_KEYBOARD_P,
        KON_KEYBOARD_Q,
        KON_KEYBOARD_R,
        KON_KEYBOARD_S,
        KON_KEYBOARD_T,
        KON_KEYBOARD_U,
        KON_KEYBOARD_V,
        KON_KEYBOARD_W,
        KON_KEYBOARD_X,
        KON_KEYBOARD_Y,
        KON_KEYBOARD_Z,
        KON_KEYBOARD_EXCLAMATION_MARK,
        KON_KEYBOARD_AT,
        KON_KEYBOARD_HASH,
        KON_KEYBOARD_DOLLAR,
        KON_KEYBOARD_MODULO,
        KON_KEYBOARD_EXPONENT,
        KON_KEYBOARD_AND,
        KON_KEYBOARD_STAR,
        KON_KEYBOARD_PARENTHESES_OPEN,
        KON_KEYBOARD_PARENTHESES_CLOSED,
        KON_KEYBOARD_RETURN,
        KON_KEYBOARD_ESCAPE,
        KON_KEYBOARD_DELETE,
        KON_KEYBOARD_TAB,
        KON_KEYBOARD_SPACE,
        KON_KEYBOARD_UNDERSCORE,
        KON_KEYBOARD_EQUAL,
        KON_KEYBOARD_BRACKET_OPEN,
        KON_KEYBOARD_BRACKET_CLOSE,
        KON_KEYBOARD_BACK_SLASH,
        KON_KEYBOARD_SQUARE,
        KON_KEYBOARD_SEMICOLON,
        KON_KEYBOARD_QUOTE,
        KON_KEYBOARD_TILDE,
        KON_KEYBOARD_LESS_THAN,
        KON_KEYBOARD_MORE_THAN,
        KON_KEYBOARD_FORWARD_SLASH,
        KON_KEYBOARD_CAPS_LOCK,
        KON_KEYBOARD_F1,
        KON_KEYBOARD_F2,
        KON_KEYBOARD_F3,
        KON_KEYBOARD_F4,
        KON_KEYBOARD_F5,
        KON_KEYBOARD_F6,
        KON_KEYBOARD_F7,
        KON_KEYBOARD_F8,
        KON_KEYBOARD_F9,
        KON_KEYBOARD_F10,
        KON_KEYBOARD_F11,
        KON_KEYBOARD_F12,
        KON_KEYBOARD_PRINT_SCREEN,
        KON_KEYBOARD_SCROLL_LOCK,
        KON_KEYBOARD_PAUSE,
        KON_KEYBOARD_INSERT,
        KON_KEYBOARD_HOME,
        KON_KEYBOARD_PAGE_UP,
        KON_KEYBOARD_DELETE_FORWARD,
        KON_KEYBOARD_END,
        KON_KEYBOARD_PAGE_DOWN,
        KON_KEYBOARD_RIGHT_ARROW,
        KON_KEYBOARD_LEFT_ARROW,
        KON_KEYBOARD_DOWN_ARROW,
        KON_KEYBOARD_UP_ARROW,
        KON_KEYBOARD_NUM_LOCK,
        KON_KEYBOARD_KEYPAD_FORWARD_SLASH,
        KON_KEYBOARD_KEYPAD_STAR,
        KON_KEYBOARD_KEYPAD_MINUS,
        KON_KEYBOARD_KEYPAD_PLUS,
        KON_KEYBOARD_KEYPAD_RETURN,
        KON_KEYBOARD_KEYPAD_1,
        KON_KEYBOARD_KEYPAD_2,
        KON_KEYBOARD_KEYPAD_3,
        KON_KEYBOARD_KEYPAD_4,
        KON_KEYBOARD_KEYPAD_5,
        KON_KEYBOARD_KEYPAD_6,
        KON_KEYBOARD_KEYPAD_7,
        KON_KEYBOARD_KEYPAD_8,
        KON_KEYBOARD_KEYPAD_9,
        KON_KEYBOARD_KEYPAD_0,
        KON_KEYBOARD_KEYPAD_DOT,
        KON_KEYBOARD_PIPE,
        KON_KEYBOARD_APP,
        KON_KEYBOARD_POWER,
        KON_KEYBOARD_KEYPAD_EQUAL,
        KON_KEYBOARD_F13,
        KON_KEYBOARD_F14,
        KON_KEYBOARD_F15,
        KON_KEYBOARD_F16,
        KON_KEYBOARD_F17,
        KON_KEYBOARD_F18,
        KON_KEYBOARD_F19,
        KON_KEYBOARD_F20,
        KON_KEYBOARD_F21,
        KON_KEYBOARD_F22,
        KON_KEYBOARD_F23,
        KON_KEYBOARD_F24,
        KON_KEYBOARD_EXECUTE,
        KON_KEYBOARD_HELP,
        KON_KEYBOARD_MENU,
        KON_KEYBOARD_SELECT,
        KON_KEYBOARD_STOP,
        KON_KEYBOARD_AGAIN,
        KON_KEYBOARD_UNDO,
        KON_KEYBOARD_CUT,
        KON_KEYBOARD_COPY,
        KON_KEYBOARD_PASTE,
        KON_KEYBOARD_FIND,
        KON_KEYBOARD_MUTE,
        KON_KEYBOARD_VOLUME_UP,
        KON_KEYBOARD_VOLUME_DOWN,
        KON_KEYBOARD_LOCKING_CAPS_LOCK,
        KON_KEYBOARD_LOCKING_NUM_LOCK,
        KON_KEYBOARD_LOCKING_SCROLL_LOCK,
        KON_KEYBOARD_KEYPAD_COMMA,
        KON_KEYBOARD_KEYPAD_EQSIGN,
        KON_KEYBOARD_INTER_1,
        KON_KEYBOARD_INTER_2,
        KON_KEYBOARD_INTER_3,
        KON_KEYBOARD_INTER_4,
        KON_KEYBOARD_INTER_5,
        KON_KEYBOARD_INTER_6,
        KON_KEYBOARD_INTER_7,
        KON_KEYBOARD_INTER_8,
        KON_KEYBOARD_INTER_9,
        KON_KEYBOARD_LANG_1,
        KON_KEYBOARD_LANG_2,
        KON_KEYBOARD_LANG_3,
        KON_KEYBOARD_LANG_4,
        KON_KEYBOARD_LANG_5,
        KON_KEYBOARD_LANG_6,
        KON_KEYBOARD_LANG_7,
        KON_KEYBOARD_LANG_8,
        KON_KEYBOARD_LANG_9,
        KON_KEYBOARD_ALT_ERASE,
        KON_KEYBOARD_SYSREQ,
        KON_KEYBOARD_CANCEL,
        KON_KEYBOARD_CLEAR,
        KON_KEYBOARD_PRIOR,
        KON_KEYBOARD_RETURN_ALT,
        KON_KEYBOARD_SEPARATOR,
        KON_KEYBOARD_OUT,
        KON_KEYBOARD_OPER,
        KON_KEYBOARD_CLEAR_AGAIN,
        KON_KEYBOARD_CRSEL,
        KON_KEYBOARD_EXSEL,
        /* RESERVED */
        KON_KEYBOARD_KEYPAD_00 = 0xB0,
        KON_KEYBOARD_KEYPAD_000,
        KON_KEYBOARD_THOUSANDS_SEPARATOR,
        KON_KEYBOARD_DECIMAL_SEPARATOR,
        KON_KEYBOARD_CURENCY,
        KON_KEYBOARD_CURENCY_SUB,
        KON_KEYBOARD_KEYPAD_PARENTHESES_OPEN,
        KON_KEYBOARD_KEYPAD_PARENTHESES_CLOSED,
        KON_KEYBOARD_KEYBAD_BRACES_OPEN,
        KON_KEYBOARD_KEYPAD_BRACES_CLOSED,
        KON_KEYBOARD_KEYPAD_TAB,
        KON_KEYBOARD_KEYPAD_BACKSPACE,
        KON_KEYBOARD_KEYPAD_A,
        KON_KEYBOARD_KEYPAD_B,
        KON_KEYBOARD_KEYPAD_C,
        KON_KEYBOARD_KEYPAD_D,
        KON_KEYBOARD_KEYPAD_E,
        KON_KEYBOARD_KEYPAD_F,
        KON_KEYBOARD_KEYPAD_XOR,
        KON_KEYBOARD_KEYPAD_EXPONENT,
        KON_KEYBOARD_KEYPAD_MODULO,
        KON_KEYBOARD_KEYPAD_LESS_THAN,
        KON_KEYBOARD_KEYPAD_MORE_THAN,
        KON_KEYBOARD_KEYPAD_AND,
        KON_KEYBOARD_KEYPAD_AND_DOUBLE,
        KON_KEYBOARD_KEYPAD_PIPE,
        KON_KEYBOARD_KEYPAD_PIPE_DOUBLE,
        KON_KEYBOARD_KEYPAD_COLON,
        KON_KEYBOARD_KEYPAD_HASH,
        KON_KEYBOARD_KEYPAD_SPACE,
        KON_KEYBOARD_KEYPAD_AT,
        KON_KEYBOARD_KEYPAD_EXCLAMATION_MARK,
        KON_KEYBOARD_KEYPAD_MEM_STORE,
        KON_KEYBOARD_KEYPAD_MEM_RECALL,
        KON_KEYBOARD_KEYPAD_MEM_CLEAR,
        KON_KEYBOARD_KEYPAD_MEM_ADD,
        KON_KEYBOARD_KEYPAD_MEM_SUB,
        KON_KEYBOARD_KEYPAD_MEM_MUL,
        KON_KEYBOARD_KEYPAD_MEM_DIV,
        KON_KEYBOARD_KEYPAD_PLUS_MINUS,
        KON_KEYBOARD_KEYPAD_CLEAR,
        KON_KEYBOARD_KEYPAD_CLEAR_ENTRY,
        KON_KEYBOARD_KEYPAD_BINARY,
        KON_KEYBOARD_KEYPAD_OCT,
        KON_KEYBOARD_KEYPAD_DEC,
        KON_KEYBOARD_KEYPAD_HEX,
        /* RESERVED */
        KON_KEYBOARD_LEFT_CTRL = 0xE0,
        KON_KEYBOARD_LEFT_SHIFT,
        KON_KEYBOARD_LEFT_ALT,
        KON_KEYBOARD_LEFT_META,
        KON_KEYBOARD_RIGHT_CONTROL,
        KON_KEYBOARD_RIGHT_SHIFT,
        KON_KEYBOARD_RIGHT_ALT,
        KON_KEYBOARD_RIGHT_META
        /* RESERVED */
    } KON_Keyboard;

#endif