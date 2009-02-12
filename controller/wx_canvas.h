#ifndef _WX_CANVAS_H
#define _WX_CANVAS_H    1

/**
 * @file wx_canvas.h
 * @brief The canvas where our animation is displayed.
 */

#include <wx/wx.h>
#include "animation.h"
#include "wx_frame.h"

using namespace stan;

// define a scrollable canvas for drawing onto
class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas( MyFrame *parent );

    void OnPaint(wxPaintEvent &event);
    void OnMouseMove(wxMouseEvent &event);
    void OnLeftDown(wxMouseEvent &event);
    void OnLeftUp(wxMouseEvent &event);

    // set or remove the clipping region
    void Clip(bool clip) { m_clip = clip; Refresh(); }

    void set_animation(animation* anim)
    {
        anim_ = anim;
    }

protected:
    void DrawTestLines( int x, int y, int width, wxDC &dc );

    /**
     * Rotate a list of nodes around a point and store them in another list.
     */
    void rotate_nodes(int x, int y, std::list<node*>& n1, std::list<node*>& n2);

private:
    MyFrame *m_owner;
    bool m_clip;
    bool in_grab_;
    bool in_pivot_;
    figure* grab_fig_;
    figure* rot_fig_;
    int grab_x_;
    int grab_y_;
    animation* anim_;
    std::list<int> pivot_nodes_;
    int pivot_point_;
    int selected_;

    DECLARE_EVENT_TABLE()
};

#endif  // _WX_CANVAS_H
