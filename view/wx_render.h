#ifndef _WX_RENDER_H
#define _WX_RENDER_H   1

/**
 * @file wx_render.h
 * @brief Rendering routines for wxWidgets.
 */

#include <list>
#include "animation.h"
#include "trig.h"

// forward declarations
class wxColour;
class wxDC;
class wxRect;
class wxImage;

namespace stan {

class WxRender
{
public:
    /**
     * Set a wxWidgets color from a stan model color (int)
     */
    static void set_wx_color(int color, wxColour& wx_color);

    /**
     * Renders a set of nodes from a given figure within a rectangle.
     */
    static void render_nodes(figure* fig, std::list<int>& nodes, wxDC& dc, wxRect& rc);

    /**
     * Render an image in a rotated rectangle defined by two points.
     */
    static void render_image(wxImage* image, wxDC& dc, Point& p0, Point& p1);

    /**
     * Renders a figure and it's contained node positions within a rectangle.
     * A figure is a set of nodes and a set of which define how they are connected.
     */
    static void render_figure(figure* fig, wxDC& dc, wxRect& rc, bool draw_nodes = true);

    /**
     * Renders a frame within the given rectangle.
     * A frame defines a scene and includes a set of figures.
     */
    static void render_frame(frame* fr, wxDC& dc, wxRect& rc);


    /**
     * Renders an animation within the given rectangle.
     * An animation is a sequence of frames and metadata describing
     * how they are played.
     */
    static void render_animation(animation* an, wxDC& dc, wxRect& rc);

    /**
     * Utilities for caching images within animations and figures.
     */
    static int cache_anim_image(animation* an, std::string& path);
    static int cache_figure_image(figure* fig, std::string& path);
    static int cache_image(image_store* imgs, std::string& path);
    static void init_image_cache(image_store* imgs);
};

};   // namespace stan

#endif  // _WX_RENDER_H
