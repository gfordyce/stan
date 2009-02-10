#ifndef _WX_RENDER_H
#define _WX_RENDER_H   1

/**
 * @file wx_render.h
 * @brief Rendering routines for wxWidgets.
 */

#include <wx/wx.h>
#include "animation.h"

namespace stan {

    class WxRender
    {
        /**
         * Renders a node at it's given position within a rectangle.
         */
        static void render_node(node* n, wxPaintDC& dc, wxRect& rc);

        /**
         * Renders a figure and it's contained node positions within a rectangle.
         * A figure is a set of nodes and a set of which define how they are connected.
         */
        static void render_figure(figure* fig, wxPaintDC& dc, wxRect& rc);


        /**
         * Renders a frame within the given rectangle.
         * A frame defines a scene and includes a set of figures.
         */
        static void render_frame(frame* fr, wxPaintDC& dc, wxRect& rc);


        /**
         * Renders an animation within the given rectangle.
         * An animation is a sequence of frames and metadata describing
         * how they are played.
         */
        static void render_animation(animation* an, wxPaintDC& dc, wxRect& rc);
    };

};   // namespace stan

#endif  // _WX_RENDER_H
