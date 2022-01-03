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