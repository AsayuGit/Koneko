/*
    Koneko is a 2D/Raycast3D game engine written from scratch in ANSI C
    using SDL and libxml2. This engine is meant to allow game developpement
    for Linux, Windows and the og Xbox.

    Copyright (C) 2021-2023 Killian RAIMBAUD [Asayu] (killian.rai@gmail.com)

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

#ifndef _KON_JOY_H
#define _KON_JOY_H

typedef enum {
    KON_NX_PLUS = 0,
    KON_NX_MINUS = 0,
    KON_NX_A = 0,
    KON_NX_B = 0,
    KON_NX_X = 0,
    KON_NX_Y = 0,
    KON_NX_L = 0,
    KON_NX_R = 0,
    KON_NX_ZL = 0,
    KON_NX_ZR = 0,
    KON_NX_LEFTSTICK = 0,
    KON_NX_RIGHTSTICK = 0
} KON_JoyButtons;

typedef enum {
    KON_NX_LEFTSTICK_X = 0,
    KON_NX_LEFTSTICK_Y = 1,
    KON_NX_RIGHTSTICK_X = 2,
    KON_NX_RIGHTSTICK_Y = 3
} KON_JoyAxis;

#endif