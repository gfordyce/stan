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
    void OnRightDown(wxMouseEvent &event);

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
    
    figure* get_figure()
    {
        return selected_fig_;
    }

    figure* get_clip_figure() { return clip_.fig_; }

    void set_animating(bool an)
    {
        animating_ = an;
        bg_image_index_ = -1;
    }

    void play_frame_audio();

    /*****************************************************************
     * Figure operations
     */

    /**
     * Make selected figure line weight thinner or thicker.
     */
    void thinner();
    void thicker();

    /**
     * Scale selected figure.
     */
    void shrink();
    void grow();

    /**
     * Rotate figure by specified angle in radians.
     */
    void rotateCW();
    void rotateCCW();
    void rotate(double angle);

    /**
     * Set the selected image. This is used for drawing image
     * edges.
     */
    void set_image(std::string& path);
    
    /**
     * Tool modes
     */
    typedef enum {
        M_SELECT,
        M_LINE,
        M_CIRCLE,
        M_SIZE,
        M_CUT,
        M_BREAK,
        M_IMAGE,
    } Mode;

    void set_mode(Mode m) { mode_ = m; }
    Mode get_mode() { return mode_; }
    void set_color(wxColour col) { sel_color_ = col; }

    /**
     * Get the currently selected color enoded as an integer
     */
    int get_int_color()
    {
        int color = 0;
        unsigned char* p_color_bytes = reinterpret_cast<unsigned char*>(&color);
        p_color_bytes[0] = sel_color_.Red();
        p_color_bytes[1] = sel_color_.Green();
        p_color_bytes[2] = sel_color_.Blue();
        return color;
    }

private:
    MyFrame *m_owner;
    bool m_clip;
    bool in_grab_;
    bool in_pivot_;
    bool in_draw_;
    bool in_stretch_;
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
    Mode mode_;             // selection, line, circle, ...
    wxColour sel_color_;
    wxImage* sel_image_ptr_;
    std::string sel_image_path_;
    DECLARE_EVENT_TABLE()
};

#endif  // _WX_CANVAS_H
