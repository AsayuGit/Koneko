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
#include "System.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

int gputc(DisplayDevice* DDevice, BitmapFont* Font, char c, unsigned int color, unsigned int x, unsigned int y){
    /* Declaration */
    SDL_Rect DstLetter, SrcLetter;

    /* Init */
    if (!DDevice){ /* DDevice check */
        printf("ERROR: Invalid DisplayDevice !\n");
        goto Exit;
    }
    if (!Font){ /* Font check */
        printf("ERROR: No Font to print with!\n");
        goto Exit;
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
        Draw(DDevice, Font->FontSurface, &SrcLetter, &DstLetter);
    } else {
        ScaledDraw(DDevice, Font->FontSurface, &SrcLetter, &DstLetter);
    }

Exit:
    return DstLetter.w;
}

Vector2i gstrlen(BitmapFont* Font, char* text, int intCharSpce){
    return gprintf(NULL, Font, text, intCharSpce, NULL);
}

/* Could use the same engine as dialogue for bonds checking */
/* Or a letter by letter mode */
Vector2i gprintf(DisplayDevice* DDevice, BitmapFont* Font, char* text, int intCharSpce, const SDL_Rect* Bounds){
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

int map(int DstA, int DstB, int SrcA, int SrcB, int Value){
    int mappedValue = DstA + ((DstB - DstA) / (SrcB - SrcA)) * (Value - SrcA);
    return (mappedValue < DstB) ? mappedValue : DstB;
}

double dmap(double DstA, double DstB, double SrcA, double SrcB, double Value){
    double mappedValue = DstA + ((DstB - DstA) / (SrcB - SrcA)) * (Value - SrcA);
    return (mappedValue < DstB) ? mappedValue : DstB;
}

char* astrcpy(char** dst, char* src){
    size_t length;

    length = strlen(src)+1;                     /* First we compute how long is the input string */
    *dst = (char*)malloc(length*sizeof(char));   /* Then we allocate the right amount of memory */
    memcpy(*dst, src, length);                  /* Finally we copy the content from the source to the destination */

    return *dst;
}

SDL_Rect RectToVieport(const SDL_Rect* InputRect, const SDL_Rect* ViewPort, const Vector2i* Position){
    SDL_Rect Return;

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

unsigned char KON_BoundVect2iToRect(Vector2i* vect, SDL_Rect* rect){
    unsigned char result;
    Vector2d vect2d = KON_Vector2iTo2d(vect);

    result = KON_BoundVect2dToRect(&vect2d, rect);
    *vect = KON_Vector2dTo2i(&vect2d);

    return result;
}

unsigned char KON_BoundVect2dToRect(Vector2d* vect, SDL_Rect* rect){
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
    *((uint64_t*)valuePointer) |= (1ul << ((dataSize * 8) - 1));
}

void KON_SetPositive(void* valuePointer, size_t dataSize) {
    *((uint64_t*)valuePointer) &= ~(1ul << ((dataSize * 8) - 1));
}

SDL_Rect KON_GetRectVectAddition(SDL_Rect rect, Vector2d vect) {
    rect.x += vect.x;
    rect.y += vect.y;
    
    return rect;
}

double KON_GetVectNorm(Vector2d vect) {
    return sqrt(vect.x * vect.x + vect.y * vect.y);
}

double KON_GetVectCrossProduct(Vector2d vect1, Vector2d vect2) {
    return (vect1.x * vect2.y) - (vect1.y * vect2.x);
}

Vector2d KON_GetVectScalarProduct(Vector2d vect, double scalar) {
    vect.x *= scalar;
    vect.y *= scalar;

    return vect;
}

Vector2d KON_GetVectScalarDivision(Vector2d vect, double scalar) {
    vect.x /= scalar;
    vect.y /= scalar;

    return vect;
}

Vector2d KON_GetVectAddition(Vector2d vect1, Vector2d vect2){
    vect1.x += vect2.x;
    vect1.y += vect2.y;

    return vect1;
}

Vector2d KON_GetVectScalarAddition(Vector2d vect, double scalar){
    vect.x += scalar;
    vect.y += scalar;

    return vect;
}

Vector2d KON_GetVectScalarSubstraction(Vector2d vect, double scalar){
    vect.x -= scalar;
    vect.y -= scalar;

    return vect;
}

Vector2d KON_GetVectSubstraction(Vector2d vect1, Vector2d vect2){
    vect1.x -= vect2.x;
    vect1.y -= vect2.y;

    return vect1;
}

Vector2d KON_GetVectProduct(Vector2d vect1, Vector2d vect2) {
    vect1.x *= vect2.x;
    vect1.y *= vect2.y;

    return vect1;
}


bool KON_GetLinesIntersect(Vector2d seg1Start, Vector2d seg1End, Vector2d seg2Start, Vector2d seg2End, Vector2d* intersection){
    Vector2d seg1Vect, seg2Vect, segStartDiff;
    double segVectCrossProduct, seg1IntersectPoint, seg2IntersectPoint;

    /* We compute the two segments's vectors */
    seg1Vect = KON_GetVectSubstraction(seg1End, seg1Start);
    seg2Vect = KON_GetVectSubstraction(seg2End, seg2Start);

    /* Then we compute the diff between the two vector's starting position */
    segStartDiff = KON_GetVectSubstraction(seg2Start, seg1Start);
    /* Followed by the cross product of the two */
    segVectCrossProduct = KON_GetVectCrossProduct(seg1Vect, seg2Vect);

    /* t = ( q - p ) × s / ( r × s ) */
    seg1IntersectPoint = KON_GetVectCrossProduct(segStartDiff, seg2Vect) / segVectCrossProduct;
    /* u = ( q - p ) × r / ( r × s ) */
    seg2IntersectPoint = KON_GetVectCrossProduct(segStartDiff, seg1Vect) / segVectCrossProduct;

    if ((seg1IntersectPoint >= 0 && seg1IntersectPoint <= 1) && (seg2IntersectPoint >= 0 && seg2IntersectPoint <= 1)){
        /* Segment intersection */
        if (intersection){
            /* p + t  r = q + u  s */
            *intersection = KON_GetVectAddition(KON_GetVectScalarProduct(seg1Vect, seg1IntersectPoint), seg1Start);
        }
        return true;
    }
    return false;
}

bool KON_GetLineRectIntersect(SDL_Rect rect, Vector2d segStart, Vector2d segEnd, Vector2d* intersection) {
    if (KON_GetLinesIntersect(segStart, segEnd, KON_InitVector2d(rect.x, rect.y), KON_InitVector2d(rect.x, rect.y + rect.h), intersection)){ /* Left */
        return true;
    } else if (KON_GetLinesIntersect(segStart, segEnd, KON_InitVector2d(rect.x + rect.w, rect.y), KON_InitVector2d(rect.x + rect.w, rect.y + rect.h), intersection)){ /* Right */
        return true;
    } else if (KON_GetLinesIntersect(segStart, segEnd, KON_InitVector2d(rect.x, rect.y), KON_InitVector2d(rect.x + rect.w, rect.y), intersection)){ /* Up */
        return true;
    } else if (KON_GetLinesIntersect(segStart, segEnd, KON_InitVector2d(rect.x, rect.y + rect.h), KON_InitVector2d(rect.x + rect.w, rect.y + rect.h), intersection)){ /* Down */
        return true;
    }
    return false;
}

void KON_PrintRect(SDL_Rect Rect) {
    printf("{%d, %d, %d, %d}\n", Rect.x, Rect.y, Rect.w, Rect.h);
}

/*
    KON_GetDirectionFromVector() : Returns the direction corresponding to the vector's direction
    INPUT: Vector2d vect : Vector from which we want to know the direction
    OUTPUT: Direction :  : The direction corresponding to the vector vect
*/
Direction KON_GetDirectionFromVector(Vector2d vect) {
    const double piStep = M_PI / 8.0;
    double vectAngle;
    unsigned int i;
    
    vectAngle = atan2(vect.y, vect.x);
    if (vectAngle < 0)
        vectAngle += 2 * M_PI;

    for (i = 1; i < 16; i += 2) {
        if (vectAngle < piStep * i) {
            return (i - 1) / 2;
        }
    }

    return 0;
}