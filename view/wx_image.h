#ifndef _WX_IMAGE_H
#define _WX_IMAGE_H   1

#include "trig.h"

/**
 * @file wx_image.h
 * @brief Image utilities for wxWidgets.
 */

#include <wx/wx.h>

namespace stan {

    class WxImageView
    {
	public:

        WxImageView() :
            images_()
        {
        }

        ~WxImageView()
        {
            if (images_.size())
                images_.clear();
        }

        /**
         * Renders an image between points p1 and p2, rotating as needed.
         * @param p1 The point at the bottom center of the image.
         * @param p2 The point at the top center of the image.
         * @param i_index The animation image index
         */
        void render_image(Point& p0, Point& p1, int index, wxDC& dc);

        /**
         * Load image file and construct a wxImage object. Store this object
         * at the specified index for use in animation.
         */
        wxImage* cache_image(std::string& path, int index);

        wxImage* get_image(int index);

    private:
        /**
         * Images are loaded with the animation and cached into this vector where
         * they are accessible by index.
         */
        std::vector<wxImage*> images_;  // images used in animation
    };

};   // namespace stan

#endif  // _WX_IMAGE_H
