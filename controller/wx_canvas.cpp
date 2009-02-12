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
    selected_()
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
                int xoff = fr->get_xpos();
                int yoff = fr->get_ypos();

                // BOOST_FOREACH(edge* e, f->get_edges())
                for (unsigned eindex = 0; eindex < f->get_edges().size(); eindex++) {
                    dc.SetPen( wxPen(wxT("black"), 5, wxSOLID));
                    // e->print(std::cout);

                    edge* e = f->get_edge(eindex);
                    node* n1 = f->get_node(e->get_n1());
                    node* n2 = f->get_node(e->get_n2());

                    if (e->get_type() == edge::edge_line) {
                        dc.DrawLine( xoff + n1->get_x(), yoff + n1->get_y(), xoff + n2->get_x(), yoff + n2->get_y() );
                    }
                    if (e->get_type() == edge::edge_circle) {
                        // TODO: draw a circle through points n1, n2 at 180 degrees apart.
                        dc.DrawCircle( xoff + n1->get_x(), yoff + n1->get_y() - 10, 10);
                    }
                }

                // draw the nodes
                std::cout << "There are " << f->get_nodes().size() << " nodes." << std::endl;
                for (unsigned nindex = 0; nindex < f->get_nodes().size(); nindex++) {
                    node* n = f->get_node(nindex);
                    if (f->is_root_node(nindex)) {
                        dc.SetPen( wxPen(wxT("green"), 1, wxSOLID));
                    }
                    else {
                        dc.SetPen( wxPen(wxT("red"), 1, wxSOLID));
                    }
                    dc.DrawCircle(xoff + n->get_x(), yoff + n->get_y(), 2);
                }

                // if pivoting, color pivot nodes
                if (in_pivot_)
                {
                    dc.SetPen( wxPen(wxT("blue"), 1, wxSOLID));
                    for (unsigned nindex = 0; nindex < pivot_nodes_.size(); nindex++) {
                        node* n = rot_fig_->get_node(nindex);
                        dc.DrawCircle(xoff + n->get_x(), yoff + n->get_y(), 2);
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
        node* sn = rot_fig_->get_node(selected_);
        node* pn = rot_fig_->get_node(pivot_point_);

        Point pt_ms(x, y);
        Point pt_sel(sn->get_x(), sn->get_y());
        Point pt_piv(pn->get_x(), pn->get_y());
        double theta = calc_angle(pt_piv, pt_sel, pt_ms);
        std::cout << "Pivot angle is " << rad2deg(theta) << std::endl;

#if 0
        double dx = static_cast<double>(x - pivot_point_->get_x());
        double dy = static_cast<double>(y - pivot_point_->get_y());
        double radius = sqrt((dy * dy) + (dx * dx));
        double mouse_theta = asin(dy / radius);
        double mouse_theta_degs = (mouse_theta * 180.0) / 3.141592;

        // calculate angle between pivot and selected node
        dx = static_cast<double>(selected_->get_x() - pivot_point_->get_x());
        dy = static_cast<double>(selected_->get_y() - pivot_point_->get_y());
        radius = sqrt((dy * dy) + (dx * dx));
        double sel_theta = asin(dy / radius);
        double sel_theta_degs = (sel_theta * 180.0) / 3.141592;

        // now we have a delta angle
        double delta_theta = sel_theta - mouse_theta;
        double delta_theta_degs = (delta_theta * 180.0) / 3.141592;
        // std::cout << "delta angle is " << delta_theta_degs << std::endl;

        BOOST_FOREACH(node* n, pivot_nodes_)
        {
            // calculate new x,y of this node using delta angle
            dx = static_cast<double>(n->get_x() - pivot_point_->get_x());
            dy = static_cast<double>(n->get_y() - pivot_point_->get_y());
            radius = sqrt((dy * dy) + (dx * dx));
            double new_theta = asin(dy / radius) - delta_theta;
            int new_x = static_cast<int>(cos(new_theta) * radius) + pivot_point_->get_x();
            int new_y = static_cast<int>(sin(new_theta) * radius) + pivot_point_->get_y();
            std::cout << "new radius =  " << radius << std::endl; 
            std::cout << "new_x =  " << new_x << ", new_y = " << new_y << std::endl; 

            // now calculate radius from new point and verify it hasn't changed
            dx = static_cast<double>(new_x - pivot_point_->get_x());
            dy = static_cast<double>(new_y - pivot_point_->get_y());
            radius = sqrt((dy * dy) + (dx * dx));
            std::cout << "original radius =  " << radius << std::endl; 

            // transform coordinates to 0,0 at pivot point
            delta_theta = 3.141592 / 1.0;
            double temp_x = static_cast<double>(n->get_x() - pivot_point_->get_x());
            double temp_y = static_cast<double>(n->get_y() - pivot_point_->get_y());
            double new_x = temp_x * cos(-delta_theta) - temp_y * sin(-delta_theta);
            double new_y = temp_x * sin(-delta_theta) + temp_y * cos(-delta_theta);
            // transform back
            new_x += pivot_point_->get_x();
            new_y += pivot_point_->get_y();
            std::cout << "new_x =  " << new_x << ", new_y = " << new_y << std::endl; 

            n->move_to(new_x, new_y);
        }
#endif
        Refresh();
    }
}

void MyCanvas::OnLeftDown(wxMouseEvent &event)
{
    if (anim_ != NULL)
    {
        frame* fr;
        if (anim_->get_frame_at_pos(event.m_x, event.m_y, fr))
        {
            figure* fig;
            if (fr->get_figure_at_pos(event.m_x, event.m_y, 4, fig, selected_))
            {
                // grabbed a node
                std::cout << "Grabbed a node at (" << event.m_x << ", " << event.m_y << "):" << std::endl;
                if (fig->is_root_node(selected_))
                {
                    // grabbed the root, move the figure
                    std::cout << "Grabbed figure at (" << event.m_x << ", " << event.m_y << "):" << std::endl;
                    in_grab_ = true;
                    grab_fig_ = fig;
                    rot_fig_ = new figure(*fig);    // new instance for rotation
                    fr->add_figure(rot_fig_);

                    grab_x_ = event.m_x;
                    grab_y_ = event.m_y;
                }
                // TODO: pivot at this node 
                else {
                    in_pivot_ = true;
                    pivot_nodes_.clear();
                    grab_fig_ = fig;

                    rot_fig_ = new figure(*fig);    // new instance for rotation
                    rot_fig_->get_decendants(pivot_nodes_, selected_);
                    fr->add_figure(rot_fig_);

                    pivot_nodes_.push_back(selected_);   // include this node
                    node *sn = rot_fig_->get_node(selected_);
                    pivot_point_ = sn->get_parent(); // we pivot around the selected node's parent

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
        Refresh();
    }
}

// END of this file -----------------------------------------------------------
