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

Vector2i KON_PrintEx(BitmapFont* Font, KON_Surface* target, const char* text, int intCharSpce, const bool skipWhiteSpace, const unsigned int x, const unsigned int y) {
    /* Declaration */
    unsigned int CharID = 0, sizeTmp, textColor = 0;
    Vector2i CharCoords;
    Vector2i Dimensions = {0};
    bool skip = skipWhiteSpace;

    if (!text)
        return Dimensions;

    CharCoords.x = x;
    CharCoords.y = y;

    if (skipWhiteSpace)
        for (; text[CharID] != '\0' && text[CharID] == '\n'; CharID++);

    while (text[CharID] != '\0') {
        if (skip)
            for (; text[CharID] != '\0' && text[CharID] == ' '; CharID++);

        for (; text[CharID] != '\0'; CharID++) {
            if (text[CharID] != '\n') {
                sizeTmp = KON_PrintChar(Font, target, text[CharID], textColor, CharCoords.x, CharCoords.y) + intCharSpce;
                CharCoords.x += sizeTmp;
            } else {
                if (CharCoords.x > Dimensions.x)
                    Dimensions.x = CharCoords.x;

                /* New line */
                sizeTmp = CharCoords.x - x;
                if (sizeTmp > Dimensions.x)
                    Dimensions.x = sizeTmp;

                CharCoords.y += Font->Rects[0].h + 1;
                CharCoords.x = x;
                CharID++;
                break;
            }
        }
    }

    Dimensions.y = (CharCoords.y - y) + Font->Rects[(unsigned int)text[--CharID]].h;

    sizeTmp = (CharCoords.x - x) + Font->Rects[(unsigned int)text[CharID]].w;
    if (sizeTmp > Dimensions.x)
        Dimensions.x = sizeTmp;

    return Dimensions;
}

#define KON_DEFAULT_CHAR_SPACE 1
Vector2i KON_Print(BitmapFont* Font, KON_Surface* target, const char* text, const unsigned int x, const unsigned int y) {
    return KON_PrintEx(Font, target, text, KON_DEFAULT_CHAR_SPACE, false, x, y);
}

Vector2i KON_PrintTrim(BitmapFont* Font, KON_Surface* target, const char* text, const unsigned int x, const unsigned int y) {
    return KON_PrintEx(Font, target, text, KON_DEFAULT_CHAR_SPACE, true, x, y);
}

/* TOFIX */
Vector2i KON_PrintLen(BitmapFont* Font, const char* text, int intCharSpce) {
    return KON_PrintEx(Font, NULL, text, intCharSpce, false, 0, 0);
}