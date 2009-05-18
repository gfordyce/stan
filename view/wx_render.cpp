#include "wx_render.h"

/**
 * @file wx_render.h
 * @brief implements the V in MVC for Stick Figure Animator (STAN) on wxWidgets.
 */

namespace stan {

void WxRender::render_node(node* n, wxDC& dc, wxRect& rc)
{
}

void WxRender::render_figure(figure* fig, wxDC& dc, wxRect& rc)
{
}

#define SCALE(p, s)	( static_cast<int>((static_cast<double>(p) * s)) )

void WxRender::render_frame(frame* fr, wxDC& dc, wxRect& rc)
{
	double x_scale = static_cast<double>(rc.width) / static_cast<double>(fr->get_width());
	double y_scale = static_cast<double>(rc.height) / static_cast<double>(fr->get_height());
	// std::cout << "Scale is " << x_scale << ", " << y_scale << std::endl;

    int xoff = rc.x;
    int yoff = rc.y;
	// std::cout << "The frame offset is " << xoff << ", " << yoff << std::endl;

    BOOST_FOREACH(figure* f, fr->get_figures()) {
        bool enabled = f->is_enabled();


        enabled ? dc.SetPen( wxPen(wxT("black"), 1, wxSOLID)) : dc.SetPen( wxPen(wxT("yellow"), 10, wxSOLID));
        for (unsigned eindex = 0; eindex < f->get_edges().size(); eindex++) {
            edge* e = f->get_edge(eindex);
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

                dc.DrawCircle( xoff + SCALE(cx, x_scale),
							   yoff + SCALE(cy, y_scale),
							   SCALE(radius, x_scale));
            }
        }
	}
}

void WxRender::render_animation(animation* an, wxDC& dc, wxRect& rc)
{
}

};  // namespace stan

// END of this file -----------------------------------------------------------
