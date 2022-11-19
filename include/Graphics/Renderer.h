#ifndef _RENDERER_H
#define _RENDERER_H

    typedef enum {
        RENDER_NONE,
        RENDER_2D_BITMAP,
        RENDER_2D_TILEMAP,
        RENDER_3D_RAYCAST
    } KON_Renderers;

    #include "Map.h"

    void KON_RenderLayer(MapLayer* layer);

#endif
