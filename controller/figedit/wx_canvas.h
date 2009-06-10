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
    void OnEraseBackground(wxEraseEvent& event);
    void OnMouseMove(wxMouseEvent &event);
    void OnLeftDown(wxMouseEvent &event);
    void OnLeftUp(wxMouseEvent &event);
    void OnRightDown(wxMouseEvent &event);

    // set or remove the clipping region
    void Clip(bool clip) { m_clip = clip; Refresh(); }

    void set_figure(figure* fig)
    {
        fig_ = fig;
        frame_->add_figure(fig);
    }

    figure* get_figure()
    {
        return fig_;
    }

    /**
     * Tool modes
     */
    typedef enum {
        M_SELECT,
        M_LINE,
        M_CIRCLE,
        M_SIZE,
        M_COLOR,
        M_STYLE,
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

    void set_wx_color(int color, wxColour& wx_color)
    {
        unsigned char* p_color_bytes = reinterpret_cast<unsigned char*>(&color);
        wx_color.Set(p_color_bytes[0], p_color_bytes[1], p_color_bytes[2]);
    }

    bool is_dirty() { return false; }

    /**
     * Deletes current figure and sets to NULL. An edge that is
     * created with an edge type tool will create a figure.
     */
    void new_figure();

    /**
     * Set the selected image. This is used for drawing image
     * edges.
     */
    void set_image(wxImage* image);

private:
    MyFrame *m_owner;
    bool m_clip;
    bool in_grab_;
    bool in_pivot_;
    bool in_draw_;
    bool in_stretch_;
    frame* frame_;
    figure* fig_;
    figure* grab_fig_;
    figure* pivot_fig_;     // a figure in pivot operation (a rotation from selected)
    figure* selected_fig_;     // a figure in pivot operation (a rotation from selected)
    int grab_x_;
    int grab_y_;
    std::list<int> pivot_nodes_;    // list of nodes which need to rotate
    int pivot_point_;       // the node we are pivoting about
    int selected_;          // the node that was grabbed
    Mode mode_;             // selection, line, circle, ...
    wxColour sel_color_;
    wxImage* sel_image_;

    DECLARE_EVENT_TABLE()
};

#endif  // _WX_CANVAS_H
