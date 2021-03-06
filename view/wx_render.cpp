#include "wx_render.h"
#include <wx/wx.h>
#include <wx/sound.h>

/**
 * @file wx_render.h
 * @brief implements the M(V)C for Stick Figure Animator (STAN) on wxWidgets.
 */

namespace stan {

void WxRender::set_wx_color(int color, wxColour& wx_color)
{
    unsigned char* p_color_bytes = reinterpret_cast<unsigned char*>(&color);
    wx_color.Set(p_color_bytes[0], p_color_bytes[1], p_color_bytes[2]);
}

void WxRender::render_nodes(figure* fig, std::list<int>& nodes, wxDC& dc, wxRect& rc)
{
    int xoff = rc.GetX();
    int yoff = rc.GetY();

    BOOST_FOREACH(int nindex, nodes) {
        node* n = fig->get_node(nindex);
        dc.DrawCircle(xoff + n->get_x(), yoff + n->get_y(), 2);
    }
}

void WxRender::render_image(wxImage* image, wxDC& dc, Point& p0, Point& p1)
{
    // calc the angle between the points
    double theta = calc_angle_vertical(p0, p1);
    theta = PI - theta;

    double dx = p1.x - p0.x;
    double dy = p1.y - p0.y;

    if ( dx == 0 && dy == 0 )
        return;
    
    double height = sqrt((dx * dx) + (dy * dy));
    double image_width = abs(image->GetWidth());
    double image_height = abs(image->GetHeight());
    double w2h_ratio = (double)image->GetWidth() / (double)image->GetHeight();
    double width = w2h_ratio * height;

    wxPoint pc((p1.x - p0.x) / 2, p1.y - p0.y);

    wxImage scale_image = image->Scale((int)width, (int)height);
    wxImage rot_image = scale_image.Rotate(theta, pc);
    wxBitmap imageBitmap(rot_image);
    wxMask* mask = new wxMask(imageBitmap, wxColour(0, 0, 0));
    imageBitmap.SetMask(mask);

    // calculate where to draw the bitmap so that n1, n2 are placed properly
    double rot_height = rot_image.GetHeight();
    double rot_width = rot_image.GetWidth();
    double l = height;
    double w = width;

    double a1 = abs(l * sin((PI / 2) - theta));
    double b1 = abs(l * cos((PI / 2) - theta));

    double a2 = abs(w * sin(theta));
    double b2 = abs(w * cos(theta));

    Point c1, c2;
    if (theta >= 0) {   // left hemisphere
        if (theta <= (PI / 2)) {
            c1.x = b1 + b2; c1.y = a1;
            c2.x = b1; c2.y = a1 + a2;
        }
        else {
            c1.x = b1 + b2; c1.y = a2;
            c2.x = b1; c2.y = 0;
        }
    }
    else {  // right hemisphere
        if (abs(theta) <= (PI / 2)) {
            c1.x = 0; c1.y = a1;
            c2.x = b2; c2.y = a1 + a2;
        }
        else {
            c1.x = b2; c1.y = 0;
            c2.x = 0; c2.y = a2;
        }
    }

    // find the midpoint to determine (x,y) shift
    Point mid;
    midpoint(c1, c2, mid);
    double ximage = p0.x - mid.x;
    double yimage = p0.y - mid.y;

    dc.DrawBitmap(imageBitmap, static_cast<wxCoord>(ximage), static_cast<wxCoord>(yimage), true);
}

void WxRender::play_frame_audio(animation* anim, frame* fr)
{
    meta_store* meta = anim->get_meta_store();
    assert(meta != NULL);

    int snd_index = fr->get_sound_index();
    if (snd_index >= 0) {
        meta_data* md = meta->get_meta_data(snd_index);
        if (md == NULL) {
            std::cout << "Sound not found for index " << snd_index << std::endl;
            return;
        }

        wxSound* sel_sound = static_cast<wxSound*>(md->get_meta_ptr());
        if (sel_sound->IsOk()) {
            sel_sound->Play(wxSOUND_SYNC);
        }
    }
}

void WxRender::render_figure(figure* fig, wxDC& dc, wxRect& rc, bool draw_nodes)
{
    bool enabled = fig->is_enabled();
    int xoff = rc.GetX();
    int yoff = rc.GetY();

    wxColour edge_color;
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    for (unsigned eindex = 0; eindex < fig->get_edges().size(); eindex++) {
        edge* e = fig->get_edge(eindex);
        if (e != NULL) {
            node* n1 = fig->get_node(e->get_n1());
            node* n2 = fig->get_node(e->get_n2());

            WxRender::set_wx_color(e->get_color(), edge_color);
            if (enabled) {
                dc.SetPen(wxPen(edge_color, fig->get_weight(), wxSOLID));
            }
            else {  // disabled, use background color
                dc.SetPen(wxPen(wxColor(136, 136, 136), fig->get_weight(), wxSOLID));
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
                    // TODO: get image index from edge, use image_view_ to render
                    Point p0(n1->get_x(), n1->get_y());
                    Point p1(n2->get_x(), n2->get_y());

                    // Look up cached image object stored in figure
                    meta_store* meta = fig->get_meta_store();
                    if (e->get_meta_index() < 0) {
                        std::cout << "Bad meta index for edge " << eindex << std::endl;
                        return;

                    }
                    meta_data* md = meta->get_meta_data(e->get_meta_index());
                    if (md == NULL) {
                        std::cout << "Image not found for index " << e->get_meta_index() << std::endl;
                        return;
                    }

                    wxImage* sel_image = static_cast<wxImage*>(md->get_meta_ptr());

                    if (sel_image != NULL) {
                        WxRender::render_image(sel_image, dc, p0, p1);
                    }
                }
            }
        }
    }

    // draw the nodes if figure is enabled
    if (enabled && draw_nodes) {
        for (unsigned nindex = 0; nindex < fig->get_nodes().size(); nindex++) {
            node* n = fig->get_node(nindex);
            if (n != NULL) {
                if (fig->is_root_node(nindex)) {
                    dc.SetPen( wxPen(wxT("green"), fig->get_weight(), wxSOLID));
                }
                else {
                    dc.SetPen( wxPen(wxT("red"), fig->get_weight(), wxSOLID));
                }
                dc.DrawCircle(xoff + n->get_x(), yoff + n->get_y(), 2);
            }
        }
    }
}

#define SCALE(p, s)	( static_cast<int>((static_cast<double>(p) * s)) )

void WxRender::render_frame(frame* fr, wxDC& dc, wxRect& rc)
{
	double x_scale = static_cast<double>(rc.width) / static_cast<double>(fr->get_width());
	double y_scale = static_cast<double>(rc.height) / static_cast<double>(fr->get_height());

    int xoff = rc.x;
    int yoff = rc.y;

    BOOST_FOREACH(figure* f, fr->get_figures()) {
        bool enabled = f->is_enabled();

        enabled ? dc.SetPen( wxPen(wxT("black"), 1, wxSOLID)) : dc.SetPen( wxPen(wxT("yellow"), 10, wxSOLID));

        for (unsigned eindex = 0; eindex < f->get_edges().size(); eindex++) {
            edge* e = f->get_edge(eindex);
            if (e != NULL) {
                node* n1 = f->get_node(e->get_n1());
                node* n2 = f->get_node(e->get_n2());

                if (e->get_type() == edge::edge_line) {
                    dc.DrawLine( xoff + SCALE(n1->get_x(), x_scale),
					             yoff + SCALE(n1->get_y(), y_scale),
							     xoff + SCALE(n2->get_x(), x_scale),
							     yoff + SCALE(n2->get_y(), y_scale) );
                }
                if (e->get_type() == edge::edge_circle) {
                    // calculate the mid-point between n1 and n2, this will be the center
                    double cx = n1->get_x() + (n2->get_x() - n1->get_x()) / 2;
                    double cy = n1->get_y() + (n2->get_y() - n1->get_y()) / 2;

                    double dx = abs(n2->get_x() - n1->get_x());
                    double dy = abs(n2->get_y() - n1->get_y());
                    double radius = sqrt((dx * dx) + (dy * dy)) / 2;

                    dc.DrawCircle( xoff + SCALE(cx, x_scale), yoff + SCALE(cy, y_scale), SCALE(radius, x_scale));
                }
            }
        }
	}
}

void WxRender::render_animation(animation* an, wxDC& dc, wxRect& rc)
{
}

int WxRender::cache_anim_metadata(animation* an, std::string& path, meta_type type)
{
    meta_store* meta = an->get_meta_store();
    return cache_metadata(meta, path, type);
}

int WxRender::cache_figure_metadata(figure* fig, std::string& path, meta_type type)
{
    meta_store* meta = fig->get_meta_store();
    return cache_metadata(meta, path, type);
}

int WxRender::cache_metadata(meta_store* meta, std::string& path, meta_type type)
{
    void* meta_ptr = NULL;
    if (type == META_IMAGE) {
        // create an image object from the path
        wxImage* image = new wxImage();
        image->LoadFile(path);
        if (image->IsOk()) {
            meta_ptr = static_cast<void*>(image);
        }
    }
    else if (type == META_SOUND) {
        // create a sound object from the path
        wxSound* sound = new wxSound(path);
        if (sound->IsOk()) {
            meta_ptr = static_cast<void*>(sound);
        }
    }

    // cache the image and save the index as selected
    int index = meta->add_meta_data(std::string(path), meta_ptr, type);
    return index;
}

void WxRender::init_meta_cache(meta_store* meta)
{
    std::vector<meta_data*> mdtable = meta->get_meta_table();
    BOOST_FOREACH(meta_data* data, mdtable) {
        if (data->get_type() == META_IMAGE) {
            wxImage* image = new wxImage();
            if (!image->LoadFile(data->get_path())) {
                delete image;
                image = NULL;
                printf("Invalid image file %s\n", data->get_path());
            }
            data->set_meta_ptr((void*)image);
        }
        else if (data->get_type() == META_SOUND) {
            wxSound* sound = new wxSound(data->get_path());
            if (!sound->IsOk()) {
                delete sound;
                sound = NULL;
                std::cout << "Invalid sound file " << data->get_path() << std::endl;
            }
            data->set_meta_ptr((void*)sound);
        }
        else {
            std::cout << "Invalid meta data type " << data->get_type() << std::endl;
        }
    }
}

void WxRender::get_bounding_rect(figure* fig, wxRect& rc)
{
    int x1 = 640;
    int y1 = 480;
    int x2 = 0;
    int y2 = 0;

    assert(fig != NULL);
    std::vector<node*> nodes = fig->get_nodes();
    for (unsigned nindex = 0; nindex < nodes.size(); nindex++) {
        node* n = nodes[nindex];
        if (n != NULL) {
            int x = n->get_x();
            if (x < x1) {
                x1 = x;
            }
            if (x > x2) {
                x2 = x;
            }
            int y = n->get_y();
            if (y < y1) {
                y1 = y;
            }
            if (y > y2) {
                y2 = y;
            }
        }
    }

    rc.SetX(x1 - 10);
    rc.SetY(y1 - 10);
    rc.SetWidth(x2 - x1 + 20);
    rc.SetHeight(y2 - y1 + 20);
}

};  // namespace stan

// END of this file -----------------------------------------------------------
