/*
    Koneko is a 2D/Raycast3D game engine written from scratch in ANSI C
    using SDL and libxml2. This engine is meant to allow game developpement
    for Linux, Windows and the og Xbox.

    Copyright (C) 2021 Killian RAIMBAUD [Asayu] (killian.rai@gmail.com)

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

#ifndef _LINKEDLIST_H
#define _LINKEDLIST_H

    #include <unistd.h> /* size_t */

    typedef struct LinkedList LinkedList;
    struct LinkedList{
        void* data;
        LinkedList* next;
    };

    LinkedList*  KON_AppendToLinkedList(LinkedList** List, void* newData, size_t dataSize);
    LinkedList*  KON_AppendRefToLinkedList(LinkedList** List, void* newDataRef);
    LinkedList*  KON_InsertIntoLinkedList(LinkedList** List, void* newData, size_t dataSize);
    void         KON_SwapItemIntoLinkedList(LinkedList** from, LinkedList** to);
    void         KON_MoveItemIntoLinkedList(LinkedList** from, LinkedList** to);
    unsigned int KON_LinkedListCount(LinkedList* List);
    void         KON_FreeLinkedList(LinkedList** List);
    void         KON_DeleteLinkedListNode(LinkedList** node);

#endif