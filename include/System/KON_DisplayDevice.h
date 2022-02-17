#ifndef _KON_DISPLAYDEVICE_H
#define _KON_DISPLAYDEVICE_H

    #include "API.h"
    #include "Rect.h"

    typedef struct {
        Vector2d position;
        Vector2d direction;
        Vector2d plane;
    } KON_Camera;

    typedef struct {
        SDL_Window *Screen;
        SDL_Renderer *Renderer;

        KON_Rect Frame[4];              /* Screen Border Frame */

        KON_Rect RenderRect;            /* Where the game is drawn on screen */
        Vector2i InternalResolution;    /* Internal render resolution of the game */
        Vector2i ScreenResolution;      /* The external resolution of the game */
        double IRScalar;                /* The scaling nessary match the external resolution */
        bool integerScalling;           /* If set to true then the output will be pixel perfect (provided the winow is big enough) */
        KON_Camera camera;              /* The in game camera */

        bool OffScreenRender;

        /* Timing control */
        uint32_t lastFrame;
        uint32_t frametime;
    } DisplayDevice;

    void KON_CreateDisplayDevice(int resX, int resY, char* gameTitle);
    void KON_FreeDisplayDevice();

    void KON_UpdateRenderRect();

    /*
        SUMMARY : Set the camera fov (in degrees)
        INPUT   : double fov : The new camera fov.
    */
    void KON_SetCameraFOV(double fov);
    void KON_CenterCameraOnCoordinates(double X, double Y);
    void KON_BoundCameraToRegion(KON_Rect Region);
    void KON_RotateCamera(double angle);

#endif