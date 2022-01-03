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

#include "RessourceManager.h"
#include "LinkedList.h"

#include <stdlib.h>
#include <string.h>

/* Loaded ressources bank */
static LinkedList* loadedRessources = NULL;

/*
    SUMMARY : Finds a managed ressource node by its key, returning NULL if not found
    INPUT   : char* key   : Ressource to search for
    OUTPUT  : LinkedList* : Ressource node or null
*/
static LinkedList* KON_SearchRessourceNodeByKey(char* key) {
    LinkedList* ressourceIterator = loadedRessources;

    while (ressourceIterator) {
        if (strcmp(((ManagedRessource*)ressourceIterator->data)->key, key) == 0)
            return ressourceIterator;
        ressourceIterator = ressourceIterator->next;
    }
    return NULL;
}

/*
    SUMMARY : Finds a managed ressource node by its contaning ressource,
              returning NULL if not found
    INPUT   : void* ressource : Ressource to search for
    OUTPUT  : LinkedList*     : Ressource node or null
*/
static LinkedList* KON_SearchRessourceNodeByRef(void* ressource) {
    LinkedList* ressourceIterator = loadedRessources;

    while (ressourceIterator) {
        if (((ManagedRessource*)ressourceIterator->data)->ressource == ressource)
            return ressourceIterator;
        ressourceIterator = ressourceIterator->next;
    }

    return NULL;
}

void KON_AddManagedRessource(char* key, void* data) {
    ManagedRessource* managedRessource = (ManagedRessource*)malloc(sizeof(ManagedRessource));

    managedRessource->key = (char*)malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(managedRessource->key, key);
    managedRessource->nbOfReferences = 1;
    managedRessource->ressource = data;
    KON_AppendRefToLinkedList(&loadedRessources, managedRessource);
}

void* KON_GetManagedRessource(char* key) {
    LinkedList* managedRessource = NULL;
    
    if (!(managedRessource = KON_SearchRessourceNodeByKey(key)))
        return NULL;
    
    /* We give away a new reference to the managed ressource so we increment the ref counter */
    ((ManagedRessource*)managedRessource->data)->nbOfReferences++;

    return ((ManagedRessource*)managedRessource->data)->ressource;
}

void* KON_FreeManagedRessourceByKey(char* key) {
    void* returnRessource = NULL;
    LinkedList* managedRessource =  NULL;
    
    if (!key)
        return NULL;
    if (!(managedRessource = KON_SearchRessourceNodeByKey(key)))
        return NULL;
    if (((ManagedRessource*)managedRessource->data)->nbOfReferences > 1) {
        ((ManagedRessource*)managedRessource->data)->nbOfReferences--;
        return NULL;
    }
    returnRessource = ((ManagedRessource*)managedRessource->data)->ressource;
    free(((ManagedRessource*)managedRessource->data)->key);
    KON_DeleteLinkedListNode(&managedRessource);

    return returnRessource;
}

void* KON_FreeManagedRessourceByRef(void* ressource) {
    void* returnRessource = NULL;
    LinkedList* managedRessource = NULL;

    if (!ressource)
        return NULL;
    if (!(managedRessource = KON_SearchRessourceNodeByRef(ressource)))
        return NULL;
    if (((ManagedRessource*)managedRessource->data)->nbOfReferences > 1) {
        ((ManagedRessource*)managedRessource->data)->nbOfReferences--;
        return NULL;
    }
    returnRessource = ((ManagedRessource*)managedRessource->data)->ressource;
    free(((ManagedRessource*)managedRessource->data)->key); /* Free the ressource container */
    KON_DeleteLinkedListNode(&managedRessource);

    return returnRessource; /* Return the internal ressource to be freed */
}