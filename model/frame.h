#ifndef _FRAME_H
#define _FRAME_H       1

#include <iostream>
#include <string>
#include <list>
#include <ext/hash_map>
#include <boost/foreach.hpp>

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/is_abstract.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/type_info_implementation.hpp>
#include <boost/serialization/extended_type_info_no_rtti.hpp>
#include <boost/serialization/export.hpp>

#include "figure.h"

namespace stan {

/**
 * A frame contains 1 or more figures. A sequence of frames comprises
 * an animation.
 */
class frame
{
public:
    frame() :
        figures_(),
        xpos_(0),
        ypos_(0),
        width_(DEFAULT_WIDTH),
        height_(DEFAULT_HEIGHT)
    {
    }

    frame(int xpos, int ypos, int width, int height) :
        figures_(),
        xpos_(xpos),
        ypos_(ypos),
        width_(width),
        height_(height)
    {
    }

    std::list<figure*>& get_figures() { return figures_; };

    int get_xpos() { return xpos_; };
    int get_ypos() { return xpos_; };
    int get_width() { return width_; };
    int get_height() { return height_; };

    void clone(frame* fr, const frame& other)
    {
        // copy easy stuff first
        fr->xpos_ = other.xpos_;
        fr->ypos_ = other.ypos_;
        fr->width_ = other.width_;
        fr->height_ = other.height_;

        // copy the list of figures
        BOOST_FOREACH(figure* fig, other.figures_) {
            figure* new_fig = new figure(*fig);
            fr->figures_.push_front(new_fig);
        }
    }

    // copy constructor
    frame(const frame& other)
    {
        clone(this, other);
    }

    // assignment operator
    frame& operator=(const frame& other)
    {
        if (this != &other)
        {
            clone(this, other);
        }
        return *this;
    }

    /**
     * A figure is placed into a frame at a specified (x,y) position.
     * @param fig The figure
     */
    void add_figure(figure* fig)
    {
        figures_.push_back(fig);
    }

    void remove_figure(figure* fig)
    {
        figures_.remove(fig);
    }

    /**
     * Move figure to the end of the list (lowest z-order)
     */
    void move_to_back(figure* fig)
    {
        figures_.remove(fig);
        figures_.push_front(fig);
    }

    /**
     * Determine if a figure exists at (x,y) within this frame. If so return a pointer
     * to it.
     * @param x The x position within the frame.
     * @param y The y position within the frame.
     * @param radius Distance from point to consider a hit
     * @param fig Return the figure if found
     * @param n Return the node clicked on within the figure
     * @return true if a figure was located at (x,y), else false (and fig will be NULL)
     */
    bool get_figure_at_pos(int x, int y, int radius, figure*& fig, int& n);

    bool is_inside(int x, int y)
    {
        if ( (x >= xpos_) && (x <= (xpos_ + width_)) && (y >= ypos_) && (y <= (ypos_ + width_)) ) {
             return true;
        }
        return false;
    }

    virtual void print(std::ostream& os) const
    {
        os << "pos (" << xpos_ << ", " << ypos_ << ")" << std::endl;
        os << "width " << width_ << ", height " << height_ << std::endl;

        os << "figures:" << std::endl;
        BOOST_FOREACH(figure* f, figures_) {
            os << f << " is " << (f->is_enabled() ? "enabled" : "disabled") << std::endl;
            os << f << ": " << *f << std::endl;
        }
    }

protected:
    friend class boost::serialization::access;
    friend std::ostream& operator<<(std::ostream &os, const frame &f);

	template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
	{
         ar & BOOST_SERIALIZATION_NVP(figures_);
         ar & BOOST_SERIALIZATION_NVP(width_);
         ar & BOOST_SERIALIZATION_NVP(height_);
    }

private:
    std::list<figure*> figures_;
    int xpos_;
    int ypos_;
    int width_;
    int height_;

    static const int DEFAULT_WIDTH = 100;
    static const int DEFAULT_HEIGHT = 100;
};

BOOST_CLASS_EXPORT(frame);

};  // namespace stan

#endif  // _FRAME_H
