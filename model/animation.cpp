#include "animation.h"

namespace stan {

std::ostream& operator<<(std::ostream &os, const animation &a)
{
    a.print(os);
    return os;
}

bool animation::get_frame_at_pos(int x, int y, boost::shared_ptr<frame>& fr)
{
    bool found = false;
    BOOST_FOREACH(boost::shared_ptr<frame> f, frames_)
    {
        if (f->is_inside(x, y))
        {
            fr = f;
            found = true;
            break;
        }
    }
    return found;
}

};  // namespace stan
