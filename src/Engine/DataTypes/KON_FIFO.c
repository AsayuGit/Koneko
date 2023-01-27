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

#include "KON_FIFO.h"
#include "Log.h"

#include <string.h>

struct KON_FIFO {
    unsigned int nbOfItems;
    unsigned int maxNbOfItems;
    uint8_t* fifoPushPointer;
    uint8_t* fifoPopPointer;
    size_t itemSize;
    size_t arraySize;
    uint8_t* array;
};

KON_FIFO* KON_CreateFIFO(const unsigned int nbOfItems, const size_t itemSize) {
    KON_FIFO* newFIFO = NULL;

    if (!(newFIFO = (KON_FIFO*)malloc(sizeof(KON_FIFO)))) {
        KON_SystemMsg("(KON_CreateFIFO) Couldn't allow enough memory", MESSAGE_WARNING, 0);
        return NULL;
    }

    newFIFO->nbOfItems = 0;
    newFIFO->maxNbOfItems = nbOfItems;
    newFIFO->itemSize = itemSize;
    newFIFO->arraySize = itemSize * nbOfItems;
    if (!(newFIFO->fifoPushPointer = newFIFO->fifoPopPointer = newFIFO->array = (uint8_t*)calloc(1, newFIFO->arraySize))) {
        KON_SystemMsg("(KON_CreateFIFO) Couldn't allow enough memory", MESSAGE_WARNING, 0);
        return NULL;
    }

    return newFIFO;
}

void KON_FreeFIFO(KON_FIFO** fifo) {
    if (!fifo || !(*fifo))
        return;
    
    free((*fifo)->array);
    free(*fifo);
    *fifo = NULL;
}

bool KON_FIFOPop(KON_FIFO* fifo, void* item) {
    if (fifo->nbOfItems)
        fifo->nbOfItems--;
    else
        return false;

    if (item)
        memcpy(item, fifo->fifoPopPointer, fifo->itemSize);
    memset(fifo->fifoPopPointer, 0, fifo->itemSize);

    fifo->fifoPopPointer += fifo->itemSize;
    if (fifo->fifoPopPointer >= (fifo->array + fifo->arraySize))
        fifo->fifoPopPointer = fifo->array;

    return true;
}

bool KON_FIFOPush(KON_FIFO* fifo, void* item) {
    if (fifo->nbOfItems < fifo->maxNbOfItems)
        fifo->nbOfItems++;
    else
        return false;

    memcpy(fifo->fifoPushPointer, item, fifo->itemSize);

    fifo->fifoPushPointer += fifo->itemSize;
    if (fifo->fifoPushPointer >= (fifo->array + fifo->arraySize))
        fifo->fifoPushPointer = fifo->array;

    return true;
}
