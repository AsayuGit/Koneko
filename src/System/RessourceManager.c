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

#include "RessourceManager.h"
#include "LinkedList.h"
#include "CommunFunctions.h"

#include <stdlib.h>
#include <string.h>

/* Loaded ressources bank */
static LinkedList* loadedRessources = NULL;

/*
    SUMMARY : Finds a managed ressource node by its name & type, returning NULL if not found
    INPUT   : char* name   : Ressource to search for
    OUTPUT  : LinkedList* : Ressource node or null
*/
static LinkedList** KON_SearchRessourceNodeByID(const char* name, RessourceType type) {
    LinkedList** ressourceIterator = &loadedRessources;
    ManagedRessource* ressource;

    while (*ressourceIterator) {
        ressource = (ManagedRessource*)(*ressourceIterator)->data;
        if ((strcmp(ressource->name, name) == 0) && (ressource->type == type))
            return ressourceIterator;
        ressourceIterator = &(*ressourceIterator)->next;
    }
    return NULL;
}

/*
    SUMMARY : Finds a managed ressource node by its contaning ressource,
              returning NULL if not found
    INPUT   : void* ressource : Ressource to search for
    OUTPUT  : LinkedList*     : Ressource node or null
*/
static LinkedList** KON_SearchRessourceNodeByRef(void* ressource) {
    LinkedList **ressourceIterator = &loadedRessources;

    while (*ressourceIterator) {
        if (((ManagedRessource*)(*ressourceIterator)->data)->ressource == ressource)
            return ressourceIterator;
        ressourceIterator = &(*ressourceIterator)->next;
    }

    return NULL;
}

void KON_AddManagedRessource(const char* name, RessourceType type, void* data) {
    ManagedRessource managedRessource = {0};

    astrcpy(&managedRessource.name, name);
    managedRessource.type = type;
    managedRessource.nbOfReferences = 1;
    managedRessource.ressource = data;
    KON_AppendToLinkedList(&loadedRessources, &managedRessource, sizeof(ManagedRessource));
}

void* KON_GetManagedRessource(const char* name, RessourceType type) {
    LinkedList** managedRessource = NULL;
    
    if (!(managedRessource = KON_SearchRessourceNodeByID(name, type)))
        return NULL;
    
    /* We give away a new reference to the managed ressource so we increment the ref counter */
    ((ManagedRessource*)(*managedRessource)->data)->nbOfReferences++;

    return ((ManagedRessource*)(*managedRessource)->data)->ressource;
}

void* KON_FreeManagedRessourceByID(char* name, RessourceType type) {
    void* returnRessource = NULL;
    LinkedList** managedRessource =  NULL;
    
    if (!name)
        return NULL;
    if (!(managedRessource = KON_SearchRessourceNodeByID(name, type)))
        return NULL;
    if (((ManagedRessource*)(*managedRessource)->data)->nbOfReferences > 1) {
        ((ManagedRessource*)(*managedRessource)->data)->nbOfReferences--;
        return NULL;
    }
    returnRessource = ((ManagedRessource*)(*managedRessource)->data)->ressource;
    free(((ManagedRessource*)(*managedRessource)->data)->name);
    KON_DeleteLinkedListNode(managedRessource);

    return returnRessource;
}

void* KON_FreeManagedRessourceByRef(void* ressource) {
    void* returnRessource = NULL;
    LinkedList** managedRessource = NULL;

    if (!ressource) /* If the ressource don't exist -> NULL */
        return NULL;
    if (!(managedRessource = KON_SearchRessourceNodeByRef(ressource))) /* If we can't find it -> NULL */
        return NULL;
    if (((ManagedRessource*)(*managedRessource)->data)->nbOfReferences > 1) { /* If We found it but there's more than one ref to it -> NULL */
        ((ManagedRessource*)(*managedRessource)->data)->nbOfReferences--;
        return NULL;
    }
    /* Otherwise free it */
    returnRessource = ((ManagedRessource*)(*managedRessource)->data)->ressource;
    free(((ManagedRessource*)(*managedRessource)->data)->name); /* Free the ressource container */
    KON_DeleteLinkedListNode(managedRessource);

    return returnRessource; /* Return the internal ressource to be freed */
}
