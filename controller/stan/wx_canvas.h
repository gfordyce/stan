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

typedef struct
{
    frame* fr_;
    figure* fig_;
} clipboard;

// define a scrollable canvas for drawing onto
class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas(wxWindow *parent, wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

    void OnPaint(wxPaintEvent &event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnMouseMove(wxMouseEvent &event);
    void OnLeftDown(wxMouseEvent &event);
    void OnLeftUp(wxMouseEvent &event);

    // set or remove the clipping region
    void Clip(bool clip) { m_clip = clip; Refresh(); }

    void set_bg_image()
    {
        assert(anim_ != NULL);

        int img_index;
        std::list<frame*> frames = anim_->get_frames();
        BOOST_FOREACH(frame* f, frames) {
            img_index = f->get_image_index();
            if (img_index >= 0) {
                bg_image_index_ = img_index;
            }
            if (f == selected_frame_)
                break;
        }
    }

    void set_frame(frame* fr)
    {
        selected_frame_ = fr;
        selected_fig_ = selected_frame_->get_first_figure();
        set_bg_image();
        Refresh();
    }

    void set_animation(animation* anim)
    {
        anim_ = anim;
        clip_.fig_ = NULL;
    }

    frame* get_frame() { return selected_frame_; }

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

    void cut_figure()
    {
        if (selected_fig_ != NULL && selected_frame_ != NULL) {
            clip_.fig_ = selected_fig_;
            selected_frame_->remove_figure(selected_fig_);
            selected_fig_ = selected_frame_->get_first_figure();
            Refresh();
        }
    }

    void copy_figure()
    {
        if (selected_fig_ != NULL) {
            clip_.fig_ = selected_fig_;
        }
    }

    void paste_figure()
    {
        if (clip_.fig_ != NULL) {
            figure* new_fig = new figure(*clip_.fig_);
            new_fig->move(50, 50);
            selected_fig_ = new_fig;
            add_figure(new_fig);
        }
    }

    void set_animating(bool an)
    {
        animating_ = an;
        bg_image_index_ = -1;
    }

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
    std::list<int> pivot_nodes_;    // list of nodes which need to rotate
    int pivot_point_;       // the node we are pivoting about
    int selected_;          // the node that was grabbed
    frame* selected_frame_;
    animation* anim_;
    bool animating_;        // true when animating (don't show nodes)
    int bg_image_index_;    // background image index when animating
    clipboard clip_;

    DECLARE_EVENT_TABLE()
};

#endif  // _WX_CANVAS_H
