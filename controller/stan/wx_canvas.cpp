#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "wx_canvas.h"
#include "wx_render.h"
#include "trig.h"

using namespace stan;

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them.
BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT  (MyCanvas::OnPaint)
    EVT_MOTION (MyCanvas::OnMouseMove)
    EVT_LEFT_DOWN (MyCanvas::OnLeftDown)
    EVT_LEFT_UP (MyCanvas::OnLeftUp)
    EVT_ERASE_BACKGROUND(MyCanvas::OnEraseBackground)
END_EVENT_TABLE()

MyCanvas::MyCanvas(wxWindow *parent, wxWindowID winid, const wxPoint& pos, const wxSize& size) :
    wxScrolledWindow(parent, winid, pos, size, wxHSCROLL | wxVSCROLL | wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN),
    in_grab_(false),
    in_pivot_(false),
    grab_fig_(),
    grab_x_(0),
    grab_y_(0),
    pivot_nodes_(),
    pivot_point_(),
    selected_(),
    selected_frame_(NULL),
    anim_(NULL),
    animating_(false),
    bg_image_index_(-1)
{
    m_owner = static_cast<MyFrame*>(parent);
    m_clip = false;
}

void MyCanvas::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    wxBufferedPaintDC pdc(this);
    wxDC &dc = pdc ;
    PrepareDC(dc);
    m_owner->PrepareDC(dc);

    if ( m_clip ) {
        dc.SetClippingRegion(100, 100, 100, 100);
    }

    dc.Clear();

    if (selected_frame_ != NULL) {

        //
        // draw the background if one is specified
        //

        // if the frame has no background but we are animating, then look for
        // the last designated background (if there was one)
        int img_index = selected_frame_->get_image_index();
        if (img_index < 0 && animating_) {
            img_index = bg_image_index_;
        }

        if (img_index != -1) {
            // Look up cached image object stored in animation
            image_store* imgs = anim_->get_image_store();
            assert(imgs != NULL);

            image_data* imgd = imgs->get_image_data(img_index);
            if (imgd == NULL) {
                std::cout << "Image not found for index " << img_index << std::endl;
                return;
            }

            bg_image_index_ = img_index;
            wxImage* sel_image = static_cast<wxImage*>(imgd->get_image_ptr());
            wxImage scale_image = sel_image->Scale(selected_frame_->get_width(), selected_frame_->get_height());
            wxBitmap imageBitmap(scale_image);
            dc.DrawBitmap(imageBitmap, static_cast<wxCoord>(selected_frame_->get_xpos()),
                          static_cast<wxCoord>(selected_frame_->get_ypos()), true);
        }

        //
        // render the figures
        //

        BOOST_FOREACH(figure* f, selected_frame_->get_figures()) {
            wxRect rc;
            rc.SetX(selected_frame_->get_xpos());
            rc.SetY(selected_frame_->get_ypos());
            WxRender::render_figure(f, dc, rc, !animating_);
            if (in_pivot_) {  // if pivoting, color pivot nodes
                dc.SetPen( wxPen(wxT("blue"), 5, wxSOLID));
                WxRender::render_nodes(pivot_fig_, pivot_nodes_, dc, rc);
            }
        }

        // draw a selection box around selected figure
        if (selected_fig_ != NULL && !animating_) {
            wxRect rc;
            WxRender::get_bounding_rect(selected_fig_, rc);
            dc.SetPen( wxPen(wxT("black"), 1, wxLONG_DASH));
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.DrawRectangle(rc);
        }
    }
}

// Empty implementation, to prevent flicker
void MyCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

void MyCanvas::OnMouseMove(wxMouseEvent &event)
{
    wxClientDC dc(this);
    PrepareDC(dc);
    m_owner->PrepareDC(dc);

    wxPoint pos = event.GetPosition();
    int x = static_cast<int>(dc.DeviceToLogicalX( pos.x ));
    int y = static_cast<int>(dc.DeviceToLogicalY( pos.y ));

#if wxUSE_STATUSBAR
    wxString str;
    str.Printf( wxT("Current mouse position: %d,%d"), (int)x, (int)y );
    m_owner->SetStatusText( str );
#endif // wxUSE_STATUSBAR

    // we have grabbed something
    if (in_grab_) {
        if (grab_fig_ != NULL) {
            grab_fig_->move(x - grab_x_, y - grab_y_);
            grab_x_ = x;
            grab_y_= y;

            Refresh();
        }
        else {
            std::cout << "Error: grab active with NULL figure." << std::endl;
        }
    }
    else if (in_pivot_) {
        // calculate angle between pivot and mouse
        node* sn = selected_fig_->get_node(selected_);
        node* pn = pivot_fig_->get_node(pivot_point_);

        Point pt_ms(x, y);
        Point pt_sel(sn->get_x(), sn->get_y());
        Point pt_piv(pn->get_x(), pn->get_y());
        double theta = calc_angle(pt_piv, pt_sel, pt_ms);
        rotate_figure(selected_fig_, pivot_fig_, pivot_point_, pivot_nodes_, theta);

        Refresh();
    }
}

void MyCanvas::OnLeftDown(wxMouseEvent &event)
{
    if (selected_frame_ != NULL) {
        figure* fig;
        if (selected_frame_->get_figure_at_pos(event.m_x, event.m_y, 8, fig, selected_)) {
            // grabbed a node
            std::cout << "Grabbed a node at (" << event.m_x << ", " << event.m_y << "):" << std::endl;
            if (fig->is_root_node(selected_)) {
                // grabbed the root, move the figure
                std::cout << "Grabbed figure at (" << event.m_x << ", " << event.m_y << "):" << std::endl;
                in_grab_ = true;
                grab_fig_ = fig;
                selected_fig_ = fig;             // original figure
                grab_x_ = event.m_x;
                grab_y_ = event.m_y;

                Refresh();  // new figure may have been selected
            }
            else {
                in_pivot_ = true;

                pivot_fig_ = new figure(*fig);    // new instance for rotation
                pivot_nodes_.clear();
                pivot_nodes_.push_back(selected_);   // include this node

                node *sn = pivot_fig_->get_node(selected_);
                pivot_point_ = sn->get_parent(); // we pivot around the selected node's parent

                selected_fig_ = fig;             // original figure
                pivot_fig_ = new figure(*selected_fig_);    // new instance for rotation
                pivot_fig_->get_decendants(pivot_nodes_, selected_);
                selected_frame_->add_figure(pivot_fig_);
                selected_frame_->move_to_back(selected_fig_);   // lowest z-order
                selected_fig_->set_enabled(false);
                pivot_fig_->set_enabled(true);

                Refresh();
            }
        }
        else {
            std::cout << "No figure found at (" << event.m_x << ", " << event.m_y << "):" << std::endl;
        }
    }
}

void MyCanvas::OnLeftUp(wxMouseEvent &event)
{
    if (in_grab_)
    {
        std::cout << "Dropped figure at: " << event.m_x << ", " << event.m_y << std::endl;
        in_grab_ = false;
    }
    if (in_pivot_)
    {
        in_pivot_ = false;
        selected_frame_->remove_figure(selected_fig_);
        delete selected_fig_;
        selected_fig_ = pivot_fig_;

        Refresh();
    }
}

// END of this file -----------------------------------------------------------
