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
    MyCanvas(wxWindow *parent, wxWindowID winid = wxID_ANY,
             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

    void OnPaint(wxPaintEvent &event);
    void OnMouseMove(wxMouseEvent &event);
    void OnLeftDown(wxMouseEvent &event);
    void OnLeftUp(wxMouseEvent &event);

    // set or remove the clipping region
    void Clip(bool clip) { m_clip = clip; Refresh(); }

    void set_animation(animation* anim)
    {
        anim_ = anim;
        selected_frame_ = anim_->get_first_frame();
    }

    animation* get_animation()
    {
        return anim_;
    }

    void set_frame(frame* fr)
    {
        selected_frame_ = fr;
        Refresh();
    }

    void add_figure(figure* fig)
    {
        if (selected_frame_ != NULL) {
            selected_frame_->add_figure(fig);
            Refresh();
        }
        else {
            std::cout << "set_figure called with no frame selected" << std::endl;
        }
    }

    /**
     * Scroll through frames with shortcut keys
     */
    frame* first_frame();
    frame* next_frame();
    frame* prev_frame();

    /**
     * Duplicate the currently selected frame and add it to the
     * animation. Make this new frame the viewed frame.
     */
    void copy_frame();

protected:
    void DrawTestLines( int x, int y, int width, wxDC &dc );

private:
    MyFrame *m_owner;
    bool m_clip;
    bool in_grab_;
    bool in_pivot_;
    figure* grab_fig_;
    figure* pivot_fig_;     // a figure in pivot operation (a rotation from selected)
    figure* selected_fig_;     // a figure in pivot operation (a rotation from selected)
    int grab_x_;
    int grab_y_;
    animation* anim_;
    std::list<int> pivot_nodes_;    // list of nodes which need to rotate
    int pivot_point_;       // the node we are pivoting about
    int selected_;          // the node that was grabbed
    frame* selected_frame_;

    DECLARE_EVENT_TABLE()
};

#endif  // _WX_CANVAS_H
