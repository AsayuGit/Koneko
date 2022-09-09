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

#ifndef _KON_FIFO_H
#define _KON_FIFO_H

    #include <stdlib.h>
    #include <stdint.h>
    #include <stdbool.h>

    typedef struct KON_FIFO KON_FIFO;

    /*
        SUMMARY : Create a FIFO of a certain size and return a pointer to it.
        INPUT   : const unsigned int nbOfItems : The number of item to allocate
                : const size_t itemSize        : The size in octet of an item
        OUTPUT  : Pointer to the FIFO or NULL on error.
    */
    KON_FIFO* KON_CreateFIFO(const unsigned int nbOfItems, const size_t itemSize);

    /*
        SUMMARY : Free a previously allocated FIFO
        INPUT   : KON_FIFO** fifo : Pointer to a fifo pointer
    */
    void      KON_FreeFIFO(KON_FIFO** fifo);

    /*
        SUMMARY : Pop an item off the fifo.
        INPUT   : KON_FIFO* fifo : Pointer to the fifo to pop from.
                : void* item     : Pointer to where to store the item in.
        OUTPUT  : bool           : Wether or not an item was popped from the fifo.
    */
    bool      KON_FIFOPop(KON_FIFO* fifo, void* item);

    /*
        SUMMARY : Push an item onto the fifo.
        INPUT   : KON_FIFO* fifo : Pointer to the fifo to push into.
                : void* item     : Pointer to the item to push into the fifo
        OUTPUT  : bool           : Wether of not the item was pushed in the fifo.
    */
    bool      KON_FIFOPush(KON_FIFO* fifo, void* item);

#endif