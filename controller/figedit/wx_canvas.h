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
    } Mode;

    void set_mode(Mode m) { mode_ = m; }
    Mode get_mode() { return mode_; }
    void set_color(wxColour col) { sel_color_ = col; }

    bool is_dirty() { return false; }

    /**
     * Deletes current figure and sets to NULL. An edge that is
     * created with an edge type tool will create a figure.
     */
    void new_figure();

private:
    MyFrame *m_owner;
    bool m_clip;
    bool in_grab_;
    bool in_pivot_;
    bool in_draw_;
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

    DECLARE_EVENT_TABLE()
};

#endif  // _WX_CANVAS_H
