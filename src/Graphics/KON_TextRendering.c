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

#include "KON_TextRendering.h"

#include <stdlib.h>
#include <string.h>

/* FIXME */
int KON_PrintChar(BitmapFont* Font, KON_Surface* target, char c, unsigned int color, unsigned int x, unsigned int y) {
    /* Declaration */
    KON_Rect DstLetter, SrcLetter;

    /* Init */
    if (!Font){ /* Font check */
        KON_SystemMsg("(gputc) No Font to print with !", MESSAGE_WARNING, 0);
        return 0;
    }

    c -= 32; /* We only want the printable characters*/
    
    if (c < 0)
        c = 0;

    DstLetter.x = x;
    DstLetter.y = y;
    DstLetter.h = Font->Rects[(int)c].h;
    DstLetter.w = Font->Rects[(int)c].w;

    SrcLetter = Font->Rects[(int)c];
    SrcLetter.y += (SrcLetter.h + 1) * color;

    /* We could optimize that when rendering a sentence TODO */
    if (!KON_SetRenderTarget(target)) {
        KON_DrawScaledSurfaceRectEx(Font->FontSurface, &SrcLetter, &DstLetter, DRAW_NO_SCALE); /* TO LOOK INTO DrawSurfaceRectEx */
        KON_SetRenderTarget(NULL);
    }

    return DstLetter.w;
}

/* Could use the same engine as dialogue for bonds checking */
/* Or a letter by letter mode */
Vector2i KON_Print(BitmapFont* Font, KON_Surface* target, const char* text, int intCharSpce, const unsigned int x, const unsigned int y) {
    /* Declaration */
    unsigned int CharID = 0, sizeTmp, DimX = 0, textColor = 0;
    size_t BufferLen = 0;
    char* Buffer;
    Vector2i CharCoords;
    Vector2i Dimensions = {0};

    CharCoords.x = x;
    CharCoords.y = y;

    if (text[CharID] == '\\') {
        /*printf("ColorChange\n");*/
        BufferLen = strcspn(&text[CharID + 1], ";");
        Buffer = malloc(sizeof(char)*BufferLen);
        strncpy(Buffer, &text[CharID + 1], BufferLen);
        textColor = atoi(Buffer);
        free(Buffer);
        CharID += BufferLen + 2;
    }

    while (text[CharID] != '\0'){
        if (text[CharID] != '\n'){
            /* FIXME : This isn't coherent anymore */

            sizeTmp = KON_PrintChar(Font, target, text[CharID], textColor, CharCoords.x, CharCoords.y) + intCharSpce;
                /* sizeTmp = Font->Rects[MAX(text[CharID] - 32, 0)].w + intCharSpce;*/
            CharCoords.x += sizeTmp;
            DimX += sizeTmp;
        } else {
            /* New line */
            sizeTmp = Font->Rects[0].h + 1;
            Dimensions.y += sizeTmp;
            CharCoords.y += sizeTmp;
            CharCoords.x -= DimX;
            
            if (DimX > (unsigned int)Dimensions.x){
                Dimensions.x = DimX - intCharSpce;
                DimX = 0;
            }
        }
        CharID++;
    }

    if (DimX > (unsigned int)Dimensions.x) Dimensions.x = DimX - intCharSpce; /* Because we don't count the lase inter char space*/

    return Dimensions;
}

Vector2i KON_PrintLen(BitmapFont* Font, char* text, int intCharSpce) {
    /* TOFIX */
    return KON_Print(Font, NULL, text, intCharSpce, 0, 0); /* dDevice = NULL was used to only get the length without printing anything */
}