/**
 * @file edge.cpp
 * @brief Implementation of edge class which represents a figure edge
 * @date 2-20-10
 * @author G. Fordyce
 */

#include "edge.h"

namespace stan {

std::ostream& operator<<(std::ostream &os, const edge &e)
{
    e.print(os);
    return os;
}

void edge::print(std::ostream& os) const
{
    os << "Edge: " << n1_ << ", " << n2_ << " of type ";
    if (type_ == edge_line) {
        os << "line";
    }
    else if (type_ == edge_circle) {
        os << "circle";
    }
    else if (type_ == edge_image) {
        os << "Image";
    }
    os << " with color " << color_ << " and name " << name_ << std::endl;
}

};  // namespace stan

// END of this file -----------------------------------------------------------
