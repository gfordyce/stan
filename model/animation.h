#ifndef _ANIMATION_H
#define _ANIMATION_H       1

#include <iostream>
#include <string>
#include <list>
#include <algorithm>
#include <hash_map>
#include <boost/foreach.hpp>

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/base_object.hpp>
//#include <boost/serialization/assume_abstract.hpp>
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

    virtual ~animation() {}

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

    void del_frame(frame* fr)
    {
        std::list<frame*>::iterator iter = std::find(frames_.begin(), frames_.end(), fr);
        if (iter != frames_.end()) {
            frames_.erase(iter);
        }
    }

    void insert_frame_after(frame* sel, frame* fr)
    {
        std::list<frame*>::iterator iter = std::find(frames_.begin(), frames_.end(), sel);
        if (iter != frames_.end()) {
            frames_.insert(iter, fr);
        }
    }

    frame* get_first_frame()
    {
        std::list<frame*>::iterator iter = frames_.begin();
        return (*iter);
    }

    frame* get_next_frame(frame* cur)
    {
        frame* next_frame = NULL;

        // find the current frame
        std::list<frame*>::iterator iter = std::find(frames_.begin(), frames_.end(), cur);
        if (iter != frames_.end()) {
            std::cout << "I found the frame in get_next_frame()." << std::endl;
            if (++iter != frames_.end()) {
                next_frame = *iter;
            }
        }
        return next_frame;
    }

    frame* get_prev_frame(frame* cur)
    {
        frame* prev_frame = NULL;

        // find the current frame
        std::list<frame*>::iterator iter = std::find(frames_.begin(), frames_.end(), cur);
        if (iter != frames_.end()) {
            std::cout << "I found the frame in get_next_frame()." << std::endl;
            if (iter != frames_.begin()) {
                iter--;
                prev_frame = *iter;
            }
        }
        return prev_frame;
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
            os << fr << std::endl;
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

//BOOST_CLASS_EXPORT(animation);

};  // namespace stan

#endif  // _ANIMATION_H
