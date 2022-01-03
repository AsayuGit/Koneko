#ifndef _RESSOURCEMANAGER_H
#define _RESSOURCEMANAGER_H

    typedef struct ManagedRessource ManagedRessource;
    struct ManagedRessource {
        char* key;                      /* A unique ID identifying the shared ressource */
        void* ressource;                /* The ressource to be shared */
        unsigned int nbOfReferences;    /* The number of time the ressource is shared */
    };

    /*
        SUMMARY : Adds a new ressource as managed ressource
        INPUT   : char* key  : The key the new ressource should be reffered as
        INPUT   : void* data : Pointer to the ressource to be managed
    */
    void  KON_AddManagedRessource(char* key, void* data);
    
    /*
        SUMMARY : Finds a managed ressource in the ressource bank, return NULL if not found.
        INPUT   : char* key : Ressource to search for
        OUTPUT  : void*     : Ressource pointer or NULL
    */
    void* KON_GetManagedRessource(char* key);
    
    /*
        SUMMARY : Free a managed ressource's container and return the contained ressource if
                said managed ressources isn't shared anymore. Otherwise decrement the
                reference counter and return NULL.
                (meaning the contained ressource shouldn't be freed yet).
        INPUT   : char* key : The managed ressource key
        OUTPUT  : void*     : The contained ressource to be freed
    */
    void* KON_FreeManagedRessourceByKey(char* key);

    /*
        SUMMARY : Free a managed ressource's container and return the contained ressource if
                said managed ressources isn't shared anymore. Otherwise decrement the
                reference counter and return NULL.
                (meaning the contained ressource shouldn't be freed yet).
        INPUT   : void* ressource : The managed ressource reference
        OUTPUT  : void*           : The contained ressource to be freed
    */
    void* KON_FreeManagedRessourceByRef(void* ressource);

#endif