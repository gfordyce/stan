#include <wx/wx.h>
#include <wx/colordlg.h>
#include <wx/dcbuffer.h>
#include "wx_canvas.h"
#include "trig.h"

using namespace stan;

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them.
BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_ERASE_BACKGROUND(MyCanvas::OnEraseBackground)
    EVT_PAINT  (MyCanvas::OnPaint)
    EVT_MOTION (MyCanvas::OnMouseMove)
    EVT_LEFT_DOWN (MyCanvas::OnLeftDown)
    EVT_LEFT_UP (MyCanvas::OnLeftUp)
    EVT_RIGHT_DOWN (MyCanvas::OnRightDown)
END_EVENT_TABLE()

MyCanvas::MyCanvas(MyFrame *parent) :
    wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                     wxHSCROLL | wxVSCROLL | wxNO_FULL_REPAINT_ON_RESIZE),
    in_grab_(false),
    in_pivot_(false),
    in_draw_(false),
    in_stretch_(false),
    frame_(new frame(0, 0, 640, 480)),
    fig_(NULL),
    grab_fig_(),
    grab_x_(0),
    grab_y_(0),
    pivot_nodes_(),
    pivot_point_(),
    selected_(),
    mode_(M_SELECT),
    sel_color_(),
    sel_image_(NULL)
{
    m_owner = parent;
    m_clip = false;
}

void MyCanvas::new_figure()
{
    std::cout << "New figure." << std::endl;
    if (fig_)
        frame_->remove_figure(fig_);
    delete fig_;
    fig_ = NULL;
    Refresh();
}

void MyCanvas::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    wxBufferedPaintDC pdc(this);

    wxDC &dc = pdc ;
    PrepareDC(dc);
    m_owner->PrepareDC(dc);

    if ( m_clip )
        dc.SetClippingRegion(100, 100, 100, 100);

    if (frame_ != NULL)
    {
        // draw shape objects
        dc.SetPen( wxPen(wxT("black"), 1, wxSOLID));
        dc.DrawRectangle(frame_->get_xpos(),
                         frame_->get_ypos(),
                         frame_->get_xpos() + frame_->get_width(),
                         frame_->get_ypos() + frame_->get_height());

        BOOST_FOREACH(figure* f, frame_->get_figures()) {
            bool enabled = f->is_enabled();
            int xoff = frame_->get_xpos();
            int yoff = frame_->get_ypos();

            wxColour edge_color;
            // enabled ? dc.SetPen( wxPen(wxT("black"), 10, wxSOLID)) : dc.SetPen( wxPen(wxT("yellow"), 10, wxSOLID));
            for (unsigned eindex = 0; eindex < f->get_edges().size(); eindex++) {
                edge* e = f->get_edge(eindex);
                node* n1 = f->get_node(e->get_n1());
                node* n2 = f->get_node(e->get_n2());

                set_wx_color(e->get_color(), edge_color);
                if (enabled) {
                    dc.SetPen(wxPen(edge_color, 10, wxSOLID));
                }
                else {  // disabled, use background color
                    dc.SetPen(wxPen(wxColor(136, 136, 136), 10, wxSOLID));
                }

                if (e->get_type() == edge::edge_line) {
                    dc.DrawLine( xoff + n1->get_x(), yoff + n1->get_y(), xoff + n2->get_x(), yoff + n2->get_y() );
                }
                else if (e->get_type() == edge::edge_circle) {
                    // calculate the mid-point between n1 and n2, this will be the center
                    double cx = n1->get_x() + (n2->get_x() - n1->get_x()) / 2;
                    double cy = n1->get_y() + (n2->get_y() - n1->get_y()) / 2;

                    double dx = abs(n2->get_x() - n1->get_x());
                    double dy = abs(n2->get_y() - n1->get_y());
                    double radius = sqrt((dx * dx) + (dy * dy)) / 2;

                    dc.DrawCircle( xoff + cx, yoff + cy, radius);
                }
                else if (e->get_type() == edge::edge_image) {
                    if (enabled) {
                        Point p0(n1->get_x(), n1->get_y());
                        Point p1(n2->get_x(), n2->get_y());

                        double theta = calc_angle_vertical(p0, p1);
                        theta = PI - theta;
                        //if (theta < 0)
                        //    theta += (2 * PI);
                        std::cout << "image rotated by " << rad2deg(theta) << std::endl;

                        double dx = abs(p1.x - p0.x);
                        double dy = abs(p1.y - p0.y);
                        double height = sqrt((dx * dx) + (dy * dy));
                        double width = ((double)sel_image_->GetHeight() / (double)sel_image_->GetWidth()) * (double)sel_image_->GetWidth();
                        std::cout << "height " << height << ", width " << width << std::endl;

                        wxPoint pc((p1.x - p0.x) / 2, (p1.y - p0.y) / 2);
                        wxImage scale_image = sel_image_->Scale((int)width, (int)height);
                        wxImage rot_image = scale_image.Rotate(theta, pc);
                        wxBitmap imageBitmap(rot_image);
                        dc.DrawBitmap(imageBitmap, static_cast<wxCoord>(p0.x), static_cast<wxCoord>(p0.y));
                    }
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
    else if (in_draw_) {
        node* sn = selected_fig_->get_node(selected_);
        if (in_stretch_) {
            // decendants move along with selected node
            int dx = x - sn->get_x();
            int dy = y - sn->get_y();
            BOOST_FOREACH(int nindex, pivot_nodes_) {
                node* cn = selected_fig_->get_node(nindex);
                cn->move(dx, dy);
            }
        }
        sn->move_to(x, y);
        Refresh();
    }
}

void MyCanvas::OnLeftDown(wxMouseEvent &event)
{
    figure* fig;
    if (frame_->get_figure_at_pos(event.m_x, event.m_y, 8, fig, selected_)) {
        std::cout << "Grabbed a node at (" << event.m_x << ", " << event.m_y << "):" << std::endl;

        /**
         * Selection mode
         */
        if (mode_ == M_SELECT) {
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
                frame_->add_figure(pivot_fig_);
                frame_->move_to_back(selected_fig_);   // lowest z-order
                selected_fig_->set_enabled(false);
                pivot_fig_->set_enabled(true);

                Refresh();
            }
        }

        /**
         * Line / circle / image / size mode
         */
        else if (mode_ == M_LINE || mode_ == M_CIRCLE || mode_ == M_IMAGE || mode_ == M_SIZE) {
            // create a new node at mouse x,y and a new edge
            std::cout << "Draw/size operation" << std::endl;
            selected_fig_ = fig;

            int eindex;
            int color = get_int_color();
            wxColour wx_color;
            set_wx_color(color, wx_color);
            std::cout << "Using color: " << (int)wx_color.Red() << ", " << (int)wx_color.Green() << ", " << (int)wx_color.Blue() << std::endl;
            if (mode_ == M_LINE) {
                eindex = fig->create_line(selected_, event.m_x, event.m_y);
                edge* e = fig->get_edge(eindex);
                e->set_color(color);
                selected_ = e->get_n2(); // save the index of the new node
            }
            else if (mode_ == M_CIRCLE) {
                eindex = fig->create_circle(selected_, event.m_x, event.m_y);
                edge* e = fig->get_edge(eindex);
                e->set_color(color);
                selected_ = e->get_n2(); // save the index of the new node
            }
            else if (mode_ == M_SIZE) {
                if (event.ControlDown()) {
                    std::cout << "Size with ctrl key." << std::endl;
                    // select node and all decendants for size
                    pivot_nodes_.clear();
                    selected_fig_->get_decendants(pivot_nodes_, selected_);
                    in_stretch_ = true;
                }
            }
            else if (mode_ == M_IMAGE) {
                eindex = fig->create_image(selected_, event.m_x, event.m_y, sel_image_);
                edge* e = fig->get_edge(eindex);
                selected_ = e->get_n2(); // save the index of the new node
            }
            in_draw_ = true;
        }

        /**
         * Break figure into two at selected node
         */
        else if (mode_ == M_BREAK) {
            std::cout << "Break operation" << std::endl;
            if (!fig->is_root_node(selected_)) {
                frame_->break_figure(fig, selected_);
                Refresh();
            }
        }

        /**
         * Color mode
         */
        else if (mode_ == M_COLOR) {
            wxColourData data;
            data.SetChooseFull(true);
            for (int i = 0; i < 16; i++) {
                wxColour colour(i*16, i*16, i*16);
                data.SetCustomColour(i, colour);
            }
              
            wxColourDialog dialog(this, &data);
            if (dialog.ShowModal() == wxID_OK) {
                wxColourData retData = dialog.GetColourData();
                sel_color_ = retData.GetColour();
                // wxColour col = retData.GetColour();
                // wxBrush brush(col, wxSOLID);
                // myWindow->SetBackground(brush);
                // myWindow->Clear();
                // myWindow->Refresh();
            }
        }
    }
    else {
        std::cout << "No figure found at (" << event.m_x << ", " << event.m_y << "):" << std::endl;
        if ( (mode_ == M_LINE || mode_ == M_CIRCLE) && (fig_ == NULL) ) {
            // new figure
            double x = static_cast<double>(event.m_x);
            double y = static_cast<double>(event.m_y);
            fig_ = new figure(x, y);
            frame_->add_figure(fig_);

            if (mode_ == M_LINE) {
                fig_->create_line(fig_->get_root(), x, y + 20);
                std::cout << "Created a line at " << x << ", " << y << std::endl;
            }
            else if (mode_ == M_CIRCLE) {
                fig_->create_circle(fig_->get_root(), x, y + 20);
                std::cout << "Created a circle at " << x << ", " << y << std::endl;
            }
            Refresh();
        }
    }
}

void MyCanvas::OnLeftUp(wxMouseEvent &event)
{
    if (in_grab_) {
        std::cout << "Dropped figure at: " << event.m_x << ", " << event.m_y << std::endl;
        in_grab_ = false;
    }

    if (in_pivot_) {
        in_pivot_ = false;
        frame_->remove_figure(selected_fig_);
        delete selected_fig_;
        selected_fig_ = NULL;

        Refresh();
    }

    if (in_draw_) {
        in_draw_ = false;
    }

    if (in_stretch_) {
        in_stretch_ = false;
    }
}

void MyCanvas::OnRightDown(wxMouseEvent &event)
{
    figure* fig;
    if (frame_->get_figure_at_pos(event.m_x, event.m_y, 8, fig, selected_)) {
        std::cout << "Right clicked on a figure." << std::endl;

        // get the parent node
        node* pn1 = fig->get_node(selected_);
        int n1 = pn1->get_parent();

        // find the edge which is defined by selected and parent nodes
        edge* e = fig->find_edge(n1, selected_);
        if (e != NULL) {
            std::cout << "Found the edge." << std::endl;
            // TODO: e->set_color(sel_color_);
        }
    }
}

// END of this file -----------------------------------------------------------
