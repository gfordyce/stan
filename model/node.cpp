/**
 * @file node.cpp
 * @brief Implementation of node class which represents a figure vertex
 * @date 2-20-10
 * @author G. Fordyce
 */

#include "node.h"

namespace stan {

std::ostream& operator<<(std::ostream &os, const node &n)
{
    n.print(os);
    return os;
}

void node::print(std::ostream& os) const
{
    os << "node at (" << get_x() << "," << get_y() << "):" << std::endl;
    os << "   parent: " << parent_ << std::endl;

    if (children_.size() > 0) {
        BOOST_FOREACH(int c, children_) {
            os << "   child: " << c << std::endl;
        }
    }
    else {
        os << "   no children" << std::endl;
    }
}

};  // namespace stan

// END of this file -----------------------------------------------------------
