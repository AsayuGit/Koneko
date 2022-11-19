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

#ifndef _KON_STACK_H
#define _KON_STACK_H

    #include <stdlib.h>
    #include <stdint.h>
    #include "Bool.h"

    typedef struct KON_Stack KON_Stack;

    /*
        SUMMARY : Create a Stack of a certain size and return a pointer to it.
        INPUT   : const unsigned int nbOfItems : The number of item to allocate
                : const size_t itemSize        : The size in octet of an item
        OUTPUT  : KON_Stack*                   : Pointer to the Stack or NULL on error.
    */
    KON_Stack* KON_CreateStack(const unsigned int nbOfItems, const size_t itemSize);

    /*
        SUMMARY : Free a previously allocated Stack
        INPUT   : KON_Stack** stack : Pointer to a stack pointer
    */
    void      KON_FreeStack(KON_Stack** stack);

    /*
        SUMMARY : Pop an Item off the stack.
        INPUT   : KON_Stack* stack : Pointer to the stack to pop of
                : void* item       : Pointer to where to store the item in.
        OUTPUT  : bool             : Whether or not an item was popped from the stack.
    */
    bool      KON_StackPop(KON_Stack* stack, void* item);

    /*
        SUMMARY : Push an Item on the stack.
        INPUT   : KON_Stack* stack : Pointer to the stack to push on
                : void* item       : Pointer to the item to push on the stack
        OUTPUT  : bool : Wether or not the item was push onto the stack
    */
    bool      KON_StackPush(KON_Stack* stack, void* item);

#endif
