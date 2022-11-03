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

#include "LinkedList.h"

#include <stdlib.h>
#include <string.h>

LinkedList* KON_AppendToLinkedList(LinkedList** List, void* newData, size_t dataSize) {
    if (!List)
        return NULL;

    while (*List)
        List = &(*List)->next;
    (*List) = (LinkedList*)malloc(sizeof(LinkedList));
    (*List)->data = malloc(dataSize);
    (*List)->next = NULL;

    memcpy((*List)->data, newData, dataSize);

    return *List;
}

LinkedList* KON_InsertIntoLinkedList(LinkedList** List, void* newData, size_t dataSize) {
    LinkedList* oldElement;

    if (!List)
        return NULL;

    oldElement = *List;
    (*List) = (LinkedList*)malloc(sizeof(LinkedList));
    (*List)->data = malloc(dataSize);
    (*List)->next = oldElement;

    memcpy((*List)->data, newData, dataSize);

    return *List;
}

void KON_SwapItemIntoLinkedList(LinkedList** from, LinkedList** to) {
    LinkedList* A, *B, *NA, *NB;
    
    A = *from;
    B = *to;

    NA = A->next;
    NB = B->next;

    /* Swap to and from */
    (*to)->next = (NA == B) ? A : NA;
    (*from) = B;
    (*to) = A;
    (*to)->next = NB;
}

void KON_MoveItemIntoLinkedList(LinkedList** from, LinkedList** to) {
    LinkedList* A, *NB;
    
    A = *from;
    NB = (*to)->next;

    (*from) = (*from)->next;
    (*to)->next = A;
    A->next = NB;
}

unsigned int KON_LinkedListCount(LinkedList* List){
    unsigned int nbOfElements = 0;

    while (List){
        nbOfElements++;
        List = List->next;
    }

    return nbOfElements;
}

void KON_FreeLinkedList(LinkedList** List){
    if (List){
        if (*List){
            if ((*List)->next){
                KON_FreeLinkedList(&(*List)->next);
            }
            free((*List)->data);
            free(*List);
            *List = NULL;
        }
    }
}

void KON_DeleteLinkedListNode(LinkedList** node){
    LinkedList* nextNode = (*node)->next;
    free((*node));
    *node = nextNode;
}

LinkedList* KON_GetLinkedListNodeIndex(LinkedList* list, unsigned int index) {
    unsigned int i;

    for (i = 0; i < index; i++) {
        if (list)
            list = list->next;
        else
            break;
    }

    return list;
}

LinkedList*  KON_SearchDataInLinkedList(LinkedList* list, void* data) {
    return *KON_SearchDataPointerInLinkedList(&list, data);
}

LinkedList**  KON_SearchDataPointerInLinkedList(LinkedList** list, void* data) {    
    if (!list)
        return NULL;
    while (*list){
        if ((*list)->data == data)
            return list;
        list = &(*list)->next;
    }
    
    return NULL;
}