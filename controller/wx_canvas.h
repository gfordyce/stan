#ifndef _WX_CANVAS_H
#define _WX_CANVAS_H    1

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

    void set_animation(boost::shared_ptr<animation> anim)
    {
        anim_ = anim;
    }

protected:
    void DrawTestLines( int x, int y, int width, wxDC &dc );

    /**
     * Rotate a list of nodes around a point and store them in another list.
     */
    void rotate_nodes(int x, int y, std::list< boost::shared_ptr< node > >& n1, std::list< boost::shared_ptr< node > >& n2);

private:
    MyFrame *m_owner;
    bool m_clip;
    bool in_grab_;
    bool in_pivot_;
    boost::shared_ptr<figure> grab_fig_;
    boost::shared_ptr<figure> rot_fig_;
    int grab_x_;
    int grab_y_;
    boost::shared_ptr<animation> anim_;
    std::list< boost::shared_ptr< node > > pivot_nodes_;
    boost::shared_ptr<node> pivot_point_;
    boost::shared_ptr<node> selected_;

    DECLARE_EVENT_TABLE()
};

#endif  // _WX_CANVAS_H
