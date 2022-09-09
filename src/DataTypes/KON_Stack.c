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

#include "KON_Stack.h"
#include "Log.h"

#include <string.h>

struct KON_Stack {
    unsigned int nbOfItems;
    unsigned int maxNbOfItems;
    uint8_t* stackPointer;
    size_t itemSize;
    size_t arraySize;
    uint8_t* array;
};

KON_Stack* KON_CreateStack(const unsigned int nbOfItems, const size_t itemSize) {
    KON_Stack* newStack = NULL;

    if (!(newStack = (KON_Stack*)malloc(sizeof(KON_Stack)))) {
        KON_SystemMsg("(KON_CreateStack) Couldn't allow enough memory", MESSAGE_WARNING, 0);
        return NULL;
    }

    newStack->nbOfItems = 0;
    newStack->maxNbOfItems = nbOfItems;
    newStack->itemSize = itemSize;
    newStack->arraySize = itemSize * nbOfItems;
    if (!(newStack->stackPointer = newStack->array = (uint8_t*)calloc(1, newStack->arraySize))) {
        KON_SystemMsg("(KON_CreateStack) Couldn't allow enough memory", MESSAGE_WARNING, 0);
        return NULL;
    }

    return newStack;
}

void KON_FreeStack(KON_Stack** stack) {
    if (!stack || !(*stack))
        return;
    
    free((*stack)->array);
    free(*stack);
    *stack = NULL;
}

bool KON_StackPop(KON_Stack* stack, void* item) {
    if (stack->nbOfItems)
        stack->nbOfItems--;
    else {
        return false;
    }

    stack->stackPointer -= stack->itemSize;
    if (stack->stackPointer < stack->array)
        stack->stackPointer = (stack->array + stack->arraySize - stack->itemSize);

    if (item)
        memcpy(item, stack->stackPointer, stack->itemSize);
    memset(stack->stackPointer, 0, stack->itemSize);

    return true;
}

bool KON_StackPush(KON_Stack* stack, void* item) {
    if (stack->nbOfItems < stack->maxNbOfItems)
        stack->nbOfItems++;
    else
        return false;

    memcpy(stack->stackPointer, item, stack->itemSize);

    stack->stackPointer += stack->itemSize;
    if (stack->stackPointer >= (stack->array + stack->arraySize))
        stack->stackPointer = stack->array;

    return true;
}