#include "wx_render.h"

/**
 * @file wx_render.h
 * @brief implements the V in MVC for Stick Figure Animator (STAN) on wxWidgets.
 */

namespace stan {

void WxRender::render_node(boost::shared_ptr<node>& n, wxPaintDC& dc, wxRect& rc)
{
}

void WxRender::render_figure(boost::shared_ptr<figure>& fig, wxPaintDC& dc, wxRect& rc)
{
}

void WxRender::render_frame(boost::shared_ptr<frame>& fr, wxPaintDC& dc, wxRect& rc)
{
}

void WxRender::render_animation(boost::shared_ptr<animation>& an, wxPaintDC& dc, wxRect& rc)
{
}

};  // namespace stan

// END of this file -----------------------------------------------------------
