/**
 * @file image_data.cpp
 * @brief Manage image data associated with figures and animations.
 * @date 7-13-09
 * @author G. Fordyce
 */

#include "image_data.h"

namespace stan {

std::ostream& operator<<(std::ostream &os, const image_data &imd)
{
    imd.print(os);
    return os;
}

std::ostream& operator<<(std::ostream &os, const image_store &ims)
{
    ims.print(os);
    return os;
}

}
