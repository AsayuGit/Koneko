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

#include "CommunFunctions.h"
#include "Surface.h"
#include "Log.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

int gputc(DisplayDevice* DDevice, BitmapFont* Font, char c, unsigned int color, unsigned int x, unsigned int y){
    /* Declaration */
    KON_Rect DstLetter, SrcLetter;

    /* Init */
    if (!DDevice){ /* DDevice check */
        KON_SystemMsg("(gputc) Invalid DisplayDevice !", MESSAGE_WARNING, 0);
        return 0;
    }
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

    /* Logic */
    if (DDevice->OffScreenRender){
        KON_DrawScaledSurfaceRect(DDevice, Font->FontSurface, &SrcLetter, &DstLetter);
    } else {
        KON_DrawScaledSurfaceRectEx(DDevice, Font->FontSurface, &SrcLetter, &DstLetter, DRAW_NO_SCALE);
    }

    return DstLetter.w;
}

Vector2i gstrlen(BitmapFont* Font, char* text, int intCharSpce){
    return gprintf(NULL, Font, text, intCharSpce, NULL);
}

/* Could use the same engine as dialogue for bonds checking */
/* Or a letter by letter mode */
Vector2i gprintf(DisplayDevice* DDevice, BitmapFont* Font, char* text, int intCharSpce, const KON_Rect* Bounds){
    /* Declaration */
    unsigned int CharID, sizeTmp, DimX, textColor = 0;
    size_t BufferLen = 0;
    char* Buffer;
    Vector2i CharCoords;
    Vector2i Dimensions;
 

    /* Init */
    Dimensions.x = 0;
    Dimensions.y = 0;
    DimX = 0;
    if (Bounds) { /* Bounds check */
        CharCoords.x = Bounds->x;
        CharCoords.y = Bounds->y;
    } else {
        CharCoords.x = 0;
        CharCoords.y = 0;
    }
    CharID = 0;

    /* Logic */

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
            if (DDevice){
                sizeTmp = gputc(DDevice, Font, text[CharID], textColor, CharCoords.x, CharCoords.y) + intCharSpce;
            } else {
                sizeTmp = Font->Rects[MAX(text[CharID] - 32, 0)].w + intCharSpce;
            }
            CharCoords.x += sizeTmp;
            DimX += sizeTmp;
        } else {
            /* New line */
            sizeTmp = Font->Rects[0].h + 1;
            Dimensions.y += sizeTmp;
            CharCoords.y += sizeTmp;
            CharCoords.x -= DimX;
            
            if (DimX > Dimensions.x){
                Dimensions.x = DimX - intCharSpce;
                DimX = 0;
            }
        }
        CharID++;
    }

    if (DimX > Dimensions.x) Dimensions.x = DimX - intCharSpce; /* Because we don't count the lase inter char space*/

    return Dimensions;
}

char* astrcpy(char** dst, char* src){
    size_t length;

    length = strlen(src)+1;                     /* First we compute how long is the input string */
    *dst = (char*)malloc(length*sizeof(char));   /* Then we allocate the right amount of memory */
    memcpy(*dst, src, length);                  /* Finally we copy the content from the source to the destination */

    return *dst;
}

KON_Rect RectToVieport(const KON_Rect* InputRect, const KON_Rect* ViewPort, const Vector2i* Position){
    KON_Rect Return;

    Return.x = InputRect->x + Position->x - ViewPort->x;
    Return.y = InputRect->y + Position->y - ViewPort->y;
    Return.w = InputRect->w;
    Return.h = InputRect->h;

    return Return;
}

Vector2d KON_Vector2iTo2d(Vector2i* vect){
    return KON_InitVector2d(vect->x, vect->y);
}

Vector2i KON_Vector2dTo2i(Vector2d* vect){
    return KON_InitVector2i(vect->x, vect->y);
}

unsigned char KON_BoundVect2iToRect(Vector2i* vect, KON_Rect* rect){
    unsigned char result;
    Vector2d vect2d = KON_Vector2iTo2d(vect);

    result = KON_BoundVect2dToRect(&vect2d, rect);
    *vect = KON_Vector2dTo2i(&vect2d);

    return result;
}

unsigned char KON_BoundVect2dToRect(Vector2d* vect, KON_Rect* rect){
    unsigned char result = 0;

    if (vect->x > rect->x + rect->w){
        vect->x = rect->x + rect->w;
        result |= 1;
    } else if (vect->x < rect->x){
        vect->x = rect->x;
        result |= 1;
    }

    if (vect->y > rect->y + rect->h){
        vect->y = rect->y + rect->h;
        result |= 2;
    } else if (vect->y < rect->y){
        vect->y = rect->y;
        result |= 2;
    }
    
    return result;
}

void KON_SetNegative(void* valuePointer, size_t dataSize) {
    *((Uint64*)valuePointer) |= (1ul << ((dataSize * 8) - 1));
}

void KON_SetPositive(void* valuePointer, size_t dataSize) {
    *((Uint64*)valuePointer) &= ~(1ul << ((dataSize * 8) - 1));
}