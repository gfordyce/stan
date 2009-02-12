#ifndef _ANIMATION_H
#define _ANIMATION_H       1

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

#include "frame.h"

namespace stan {

/**
 * An animation is a sequence of frames.
 */
class animation
{
public:
    animation() :
        xpos_(0),
        ypos_(0),
        frames_()
    {
    }

    animation(int xpos, int ypos) :
        xpos_(xpos),
        ypos_(ypos),
        frames_()
    {
    }

    // Accessors
    std::list<frame*>& get_frames() { return frames_; };
    int get_xpos() { return xpos_; };
    int get_ypos_() { return ypos_; }

    /**
     * Add a frame to the animation
     * @param frame The frame
     */
    void add_frame(frame* fr)
    {
        frames_.push_back(fr);
    }

    /**
     * Is there a frame at the given position?
     */
    bool get_frame_at_pos(int x, int y, frame*& fr);

    virtual void print(std::ostream& os) const
    {
        os << "Frames:" << std::endl;
        BOOST_FOREACH(frame* fr, frames_)
        {
            os << fr << ": " << *fr << std::endl;
        }
    }

protected:
    friend class boost::serialization::access;
    friend std::ostream& operator<<(std::ostream &os, const animation &a);

	template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
	{
         ar & BOOST_SERIALIZATION_NVP(frames_);
    }

private:
    int xpos_;
    int ypos_;
    std::list<frame*> frames_;
};

BOOST_CLASS_EXPORT(animation);

};  // namespace stan

#endif  // _ANIMATION_H
