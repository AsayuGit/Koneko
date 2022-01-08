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

#ifndef _LOG_H
#define _LOG_H

    typedef enum {
        MESSAGE_LOG,
        MESSAGE_WARNING,
        MESSAGE_ERROR,
        MESSAGE_DEBUG
    } MessageGravity;

    /*
        SUMMARY : Logs a message to the console with the appropriate gravity level.
                  Will terminate the application when gravity = MESSAGE_ERROR.
        INPUT   : char* message          : The message to be printed.
        INPUT   : MessageGravity gravity : The gravity level of the messsage.
        INPUT   : unsigned int nbExt     : The number of message extensions.
        INPUT   : ...                    : nbExt number of string to be printed after.
    */
    void KON_SystemMsg(const char* message, MessageGravity gravity, unsigned int nbExt, ...);

#endif