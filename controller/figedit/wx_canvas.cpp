#include <wx/wx.h>
#include <wx/colordlg.h>
#include <wx/dcbuffer.h>
#include "wx_canvas.h"
#include "wx_render.h"
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
    sel_image_ptr_(NULL),
    sel_image_path_()
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

    if ( m_clip ) {
        dc.SetClippingRegion(100, 100, 100, 100);
    }

    if (frame_ != NULL) {
        // draw a border around the frame
        dc.SetPen( wxPen(wxT("black"), 1, wxSOLID));
        dc.DrawRectangle(frame_->get_xpos(),
                         frame_->get_ypos(),
                         frame_->get_xpos() + frame_->get_width(),
                         frame_->get_ypos() + frame_->get_height());

        // render the figures
        BOOST_FOREACH(figure* f, frame_->get_figures()) {
            wxRect rc;
            rc.SetX(frame_->get_xpos());
            rc.SetY(frame_->get_ypos());
            WxRender::render_figure(f, dc, rc);
            if (in_pivot_) {  // if pivoting, color pivot nodes
                dc.SetPen( wxPen(wxT("blue"), f->get_weight(), wxSOLID));
                WxRender::render_nodes(pivot_fig_, pivot_nodes_, dc, rc);
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
        node* sn = fig_->get_node(selected_);
        node* pn = pivot_fig_->get_node(pivot_point_);

        Point pt_ms(x, y);
        Point pt_sel(sn->get_x(), sn->get_y());
        Point pt_piv(pn->get_x(), pn->get_y());
        double theta = calc_angle(pt_piv, pt_sel, pt_ms);
        rotate_figure(fig_, pivot_fig_, pivot_point_, pivot_nodes_, theta);

        Refresh();
    }
    else if (in_draw_) {
        node* sn = fig_->get_node(selected_);
        if (in_stretch_) {
            // decendants move along with selected node
            int dx = x - sn->get_x();
            int dy = y - sn->get_y();
            BOOST_FOREACH(int nindex, pivot_nodes_) {
                node* cn = fig_->get_node(nindex);
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
                pivot_fig_->get_decendants(pivot_nodes_, selected_);
                frame_->add_figure(pivot_fig_);
                fig_ = fig;     // save the original figure
                frame_->move_to_back(fig_);   // lowest z-order
                fig_->set_enabled(false);
                pivot_fig_->set_enabled(true);
                std::cout << "Pivot figure:" << *pivot_fig_ << std::endl;

                Refresh();
            }
        }

        /**
         * Line / circle / image / size mode
         */
        else if (mode_ == M_LINE || mode_ == M_CIRCLE || mode_ == M_IMAGE || mode_ == M_SIZE) {
            // create a new node at mouse x,y and a new edge
            std::cout << "Draw/size operation" << std::endl;
            fig_ = fig;

            int eindex;
            int color = get_int_color();
            wxColour wx_color;
            WxRender::set_wx_color(color, wx_color);
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
                    fig->get_decendants(pivot_nodes_, selected_);
                    in_stretch_ = true;
                }
            }
            else if (mode_ == M_IMAGE) {
                if (sel_image_ptr_ != NULL) {
                    meta_store* meta = fig_->get_meta_store();
                    int index = meta->add_meta_data(sel_image_path_, static_cast<void*>(sel_image_ptr_), META_IMAGE);
                    eindex = fig->create_image(selected_, event.m_x, event.m_y, index);
                    edge* e = fig->get_edge(eindex);
                    selected_ = e->get_n2(); // save the index of the new node
                }
                else {
                    std::cout << "No image selected." << std::endl;
                }
            }
            in_draw_ = true;
            Refresh();
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
        else if (mode_ == M_CUT) {
            std::cout << "Cut operation" << std::endl;
            if (!fig->is_root_node(selected_)) {
                fig->remove_nodes(selected_);
                Refresh();
            }
        }
    }
    else {
        std::cout << "No figure found at (" << event.m_x << ", " << event.m_y << "):" << std::endl;
        if ( (mode_ == M_LINE || mode_ == M_CIRCLE || mode_ == M_IMAGE) && (fig_ == NULL) ) {
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
            else if (mode_ == M_IMAGE) {
                if (sel_image_ptr_ != NULL) {
                    meta_store* meta = fig_->get_meta_store();
                    int index = meta->add_meta_data(sel_image_path_, static_cast<void*>(sel_image_ptr_), META_IMAGE);
                    int eindex = fig_->create_image(fig_->get_root(), x, y - 50, index);
                    edge* e = fig_->get_edge(eindex);
                    selected_ = e->get_n2(); // save the index of the new node
                }
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
        frame_->remove_figure(fig_);
        // delete fig_; // TODO: why does this break the cloned figure?
        fig_ = pivot_fig_;

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

void MyCanvas::thinner()
{
    std::cout << "Thinner lines." << std::endl;
    if (fig_ != NULL) {
        if (fig_->thinner()) {
            Refresh();
        }
    }
}

void MyCanvas::thicker()
{
    std::cout << "Thicker lines." << std::endl;
    if (fig_ != NULL) {
        if (fig_->thicker()) {
            Refresh();
        }
    }
}

void MyCanvas::set_image(std::string& path)
{
    // create an image object from the path
    sel_image_ptr_ = new wxImage();
    sel_image_ptr_->LoadFile(path);
    sel_image_path_ = path;

    // cache the image and save the index as selected
    // image_store* imgs = fig_->get_image_store();
    // int index = imgs->add_image_data(path, static_cast<void*>(image));
    // sel_image_ = index;
}

void MyCanvas::shrink()
{
    std::cout << "Shrink figure." << std::endl;
    fig_->scale(.8);

    Refresh();
}

void MyCanvas::grow()
{
    std::cout << "Grow figure." << std::endl;
    fig_->scale(1.2);
    Refresh();
}

void MyCanvas::rotate(double angle)
{
    std::cout << "Rotate figure." << std::endl;

    figure* rot_fig = new figure(*fig_);    // new instance for rotation
    std::list<int> nodes;
    fig_->get_decendants(nodes, fig_->get_root());

    // void rotate_figure(figure* fig_, figure *dst_fig, int origin_node, std::list<int> rot_nodes, double angle)
    rotate_figure(fig_, rot_fig, fig_->get_root(), nodes, angle);
    
    frame_->remove_figure(fig_);
    delete fig_;
    fig_ = rot_fig;
    frame_->add_figure(fig_);

    Refresh();
}

void MyCanvas::rotateCW()
{
    rotate(PI / 8);
}

void MyCanvas::rotateCCW()
{
    rotate(PI / -8);
}

// END of this file -----------------------------------------------------------
