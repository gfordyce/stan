#include <wx/wx.h>
#include "wx_canvas.h"
#include "trig.h"

using namespace stan;

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them.
BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT  (MyCanvas::OnPaint)
    EVT_MOTION (MyCanvas::OnMouseMove)
    EVT_LEFT_DOWN (MyCanvas::OnLeftDown)
    EVT_LEFT_UP (MyCanvas::OnLeftUp)
END_EVENT_TABLE()

MyCanvas::MyCanvas(MyFrame *parent) :
    wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                     wxHSCROLL | wxVSCROLL | wxNO_FULL_REPAINT_ON_RESIZE),
    in_grab_(false),
    in_pivot_(false),
    grab_fig_(),
    grab_x_(0),
    grab_y_(0),
    anim_(),
    pivot_nodes_(),
    pivot_point_(),
    selected_(),
    selected_frame_(NULL)
{
    m_owner = parent;
    m_clip = false;
}

void MyCanvas::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    wxPaintDC pdc(this);

    wxDC &dc = pdc ;

    PrepareDC(dc);

    m_owner->PrepareDC(dc);

    if ( m_clip )
        dc.SetClippingRegion(100, 100, 100, 100);

    dc.Clear();

    if (anim_ != NULL)
    {
        // draw shape objects
        int frame_num = 0;
        BOOST_FOREACH(frame* fr, anim_->get_frames()) {
            dc.SetPen( wxPen(wxT("black"), 1, wxSOLID));
            dc.DrawRectangle(fr->get_xpos(), fr->get_ypos(), fr->get_xpos() + fr->get_width(), fr->get_ypos() + fr->get_height());
            BOOST_FOREACH(figure* f, fr->get_figures()) {
                bool enabled = f->is_enabled();
                int xoff = fr->get_xpos();
                int yoff = fr->get_ypos();

                enabled ? dc.SetPen( wxPen(wxT("black"), 10, wxSOLID)) : dc.SetPen( wxPen(wxT("yellow"), 10, wxSOLID));
                for (unsigned eindex = 0; eindex < f->get_edges().size(); eindex++) {
                    edge* e = f->get_edge(eindex);
                    node* n1 = f->get_node(e->get_n1());
                    node* n2 = f->get_node(e->get_n2());

                    if (e->get_type() == edge::edge_line) {
                        dc.DrawLine( xoff + n1->get_x(), yoff + n1->get_y(), xoff + n2->get_x(), yoff + n2->get_y() );
                    }
                    if (e->get_type() == edge::edge_circle) {
                        // calculate the mid-point between n1 and n2, this will be the center
                        double cx = n1->get_x() + (n2->get_x() - n1->get_x()) / 2;
                        double cy = n1->get_y() + (n2->get_y() - n1->get_y()) / 2;

                        double dx = abs(n2->get_x() - n1->get_x());
                        double dy = abs(n2->get_y() - n1->get_y());
                        double radius = sqrt((dx * dx) + (dy * dy)) / 2;

                        dc.DrawCircle( xoff + cx, yoff + cy, radius);
                    }
                }

                // draw the nodes if figure is enabled
                if (enabled) {
                    for (unsigned nindex = 0; nindex < f->get_nodes().size(); nindex++) {
                        node* n = f->get_node(nindex);
                        if (f->is_root_node(nindex)) {
                            dc.SetPen( wxPen(wxT("green"), 5, wxSOLID));
                        }
                        else {
                            dc.SetPen( wxPen(wxT("red"), 5, wxSOLID));
                        }
                        dc.DrawCircle(xoff + n->get_x(), yoff + n->get_y(), 2);
                    }

                    // if pivoting, color pivot nodes
                    if (in_pivot_)
                    {
                        dc.SetPen( wxPen(wxT("blue"), 5, wxSOLID));
                        BOOST_FOREACH(int nindex, pivot_nodes_) {
                            node* n = pivot_fig_->get_node(nindex);
                            dc.DrawCircle(xoff + n->get_x(), yoff + n->get_y(), 2);
                        }
                    }
                }
            }
            frame_num++;
        }
    }
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
    if (anim_ != NULL) {
        if (anim_->get_frame_at_pos(event.m_x, event.m_y, selected_frame_)) {
            figure* fig;
            if (selected_frame_->get_figure_at_pos(event.m_x, event.m_y, 8, fig, selected_)) {
                // grabbed a node
                std::cout << "Grabbed a node at (" << event.m_x << ", " << event.m_y << "):" << std::endl;
                if (fig->is_root_node(selected_)) {
                    // grabbed the root, move the figure
                    std::cout << "Grabbed figure at (" << event.m_x << ", " << event.m_y << "):" << std::endl;
                    in_grab_ = true;
                    grab_fig_ = fig;
                    grab_x_ = event.m_x;
                    grab_y_ = event.m_y;
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
        else {
            std::cout << "No frame found at (" << event.m_x << ", " << event.m_y << "):" << std::endl;
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
        selected_fig_ = NULL;

        Refresh();
    }
}

// END of this file -----------------------------------------------------------
