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

#include "Log.h"

#include <stdlib.h>
#include <stdio.h>
#include "Bool.h"
#include <stdarg.h>
#include "System.h"

static char* messageLeader[] = {
    "[Message]",
    "[Warning]",
    "[Error]",
    "[Debug]"
};

void KON_SystemMsg(const char* message, MessageGravity gravity, unsigned int nbExt, ...) {
    bool exitAfferPrint = false;
    char* extension;
    unsigned int i;
    va_list args;

    /* Decode the message gravity level */
    switch (gravity) {
        default:
        case MESSAGE_LOG:
            break;
        case MESSAGE_WARNING:
            break;
        case MESSAGE_ERROR:
            KON_ShowDebugScreen();
            exitAfferPrint = true;
            break;
    }
    
    printf("{%u} %s %s", KON_GetMs(), messageLeader[gravity], message);

    va_start(args, nbExt);
    for (i = 0; i < nbExt; i++) {
        extension = va_arg(args, char*);
        printf(" %s", extension);
    }
    va_end(args);

    printf("\n");

    if (exitAfferPrint) {
        while (1)
            KON_FinishFrame();
    }
}
