/**
 * @file meta_data.cpp
 * @brief Manage meta data associated with figures and animations.
 * @date 7-13-09
 * @author G. Fordyce
 */

#include "metadata.h"

namespace stan {

std::ostream& operator<<(std::ostream &os, const meta_data &md)
{
    md.print(os);
    return os;
}

std::ostream& operator<<(std::ostream &os, const meta_store &meta)
{
    meta.print(os);
    return os;
}

}
