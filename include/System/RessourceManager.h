#ifndef _RESSOURCEMANAGER_H
#define _RESSOURCEMANAGER_H

    typedef struct ManagedRessource ManagedRessource;
    struct ManagedRessource {
        char* key;                      /* A unique ID identifying the shared ressource */
        void* ressource;                /* The ressource to be shared */
        unsigned int nbOfReferences;    /* The number of time the ressource is shared */
    };

    void  KON_AddManagedRessource(char* key, void* data);
    
    /*
        SUMMARY : Finds a managed ressource in the ressource bank, return NULL if not found.
        INPUT   : char* key : Ressource to search for
        OUTPUT  : void*     : Ressource pointer or NULL
    */
    void* KON_GetManagedRessource(char* key);
    
    void* KON_FreeManagedRessourceByKey(char* key);
    void* KON_FreeManagedRessourceByRef(void* ressource);

#endif