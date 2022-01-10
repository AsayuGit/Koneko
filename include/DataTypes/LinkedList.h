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

#ifndef _LINKEDLIST_H
#define _LINKEDLIST_H

    #include <unistd.h> /* size_t */

    typedef struct LinkedList LinkedList;
    struct LinkedList{
        void* data;
        LinkedList* next;
    };

    /*
        SUMMARY : Appends New data (copy) to the end of a LinkedList.
        INPUT   : LinkedList** List : The list to append to.
        INPUT   : void* newData     : The data to append to the LinkedList.
        INPUT   : size_t dataSize   : The size of the data to be appended to the LinkedList.
        OUTPUT  : LinkedList*       : The newly appeded node (or NULL if error).
    */
    LinkedList*  KON_AppendToLinkedList(LinkedList** List, void* newData, size_t dataSize);

    /*
        SUMMARY : Appends New data (reference) to the end of a LinkedList.
        INPUT   : LinkedList** List : The list to append to.
        INPUT   : void* newData     : The reference to append to the LinkedList.
        OUTPUT  : LinkedList*       : The newly appeded node (or NULL if error).
    */
    LinkedList*  KON_AppendRefToLinkedList(LinkedList** List, void* newDataRef);

    /*
        SUMMARY : Insert New data (copy) before a LinkedList's node.
        INPUT   : LinkedList** List : The LinkedList's node to insert new data before.
        INPUT   : void* newData     : The data to insert at the node location.
        INPUT   : size_t dataSize   : The size of the data to insert into the LinkedList.
        OUTPUT  : LinkedList*       : The newly inserted note (or NULL if error).
    */
    LinkedList*  KON_InsertIntoLinkedList(LinkedList** List, void* newData, size_t dataSize);

    /*
        SUMMARY : Insert New data (reference) before a LinkedList's node.
        INPUT   : LinkedList** List : The LinkedList's node to insert new data before.
        INPUT   : void* newRefData  : The reference to insert at the node location.
        INPUT   : size_t dataSize   : The size of the data to insert into the LinkedList.
        OUTPUT  : LinkedList*       : The newly inserted note (or NULL if error).
    */
    LinkedList*  KON_InsertRefIntoLinkedList(LinkedList** List, void* newDataRef);

    /*
        SUMMARY : Swaps two nodes positions in a LinkedList.
        INPUT   : LinkedList** from : A pointer to the fist node to swap.
        INPUT   : LinkedList** to   : A pointer to the second node to swap with.
    */
    void         KON_SwapItemIntoLinkedList(LinkedList** from, LinkedList** to);
    
    /*
        SUMMARY : Move a node after another into a LinkedList.
        INPUT   : LinkedList** from : A pointer to the node to move.
        INPUT   : LinkedList** to   : A pointer to the node to place after.
    */
    void         KON_MoveItemIntoLinkedList(LinkedList** from, LinkedList** to);
    
    /*
        SUMMARY : Return the number of items in a LinkedList.
        INPUT   : LinkedList* List : The LinkedList to count the items of.
        OUTPUT  : unsigned int      : The number of items in the LinkedList.
    */
    unsigned int KON_LinkedListCount(LinkedList* List);

    /*
        SUMMARY : Free a previously allocated LinkedList.
        INPUT   : LinkedList** List : A pointer to the LinkedList to be freed.
    */
    void         KON_FreeLinkedList(LinkedList** List);

    /*
        SUMMARY : Deletes a node of a LinkedList.
        INPUT   : LinkedList** node : A pointer to the node to delete.
    */
    void         KON_DeleteLinkedListNode(LinkedList** node);

#endif