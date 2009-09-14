#include "wx_image.h"

/**
 * @file wx_image.h
 * @brief implements image utilities for wxWidgets.
 */

namespace stan {

void WxImageView::render_image(Point& p0, Point& p1, int index, wxDC& dc)
{
    double theta = calc_angle_vertical(p0, p1);
    theta = PI - theta;

    // std::cout << "image rotated by " << rad2deg(theta) << std::endl;

    double dx = p1.x - p0.x;
    double dy = p1.y - p0.y;

    double height = sqrt((dx * dx) + (dy * dy));
    double image_width = abs(sel_image_->GetWidth());
    double image_height = abs(sel_image_->GetHeight());
    double w2h_ratio = (double)sel_image_->GetWidth() / (double)sel_image_->GetHeight();
    double width = w2h_ratio * height;

    wxPoint pc((p1.x - p0.x) / 2, p1.y - p0.y);
    wxImage scale_image = sel_image_->Scale((int)width, (int)height);
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

    Point mid;
    midpoint(c1, c2, mid);

    double ximage = p0.x - mid.x;
    double yimage = p0.y - mid.y;

    dc.DrawBitmap(imageBitmap, static_cast<wxCoord>(ximage), static_cast<wxCoord>(yimage), true);
}

wxImage* WxImageView::cache_image(std::string& path, int index)
{
    wxImage* image = new wxImage();
    image->LoadFile(path);
    images_[index] = image;
    return image;
}

};  // namespace stan

// END of this file -----------------------------------------------------------
