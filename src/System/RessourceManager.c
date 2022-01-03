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
    
    ((ManagedRessource*)managedRessource->data)->nbOfReferences++;

    return ((ManagedRessource*)managedRessource->data)->ressource;
}

void* KON_FreeManagedRessourceByKey(char* key) {
    void* returnRessource = NULL;
    LinkedList* managedRessource = KON_SearchRessourceNodeByKey(key);
    if (!managedRessource)
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
    LinkedList* managedRessource = KON_SearchRessourceNodeByRef(ressource);
    if (!managedRessource)
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