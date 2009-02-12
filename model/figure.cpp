#include "figure.h"

namespace stan {

std::ostream& operator<<(std::ostream &os, const node &n)
{
    n.print(os);
    return os;
}

std::ostream& operator<<(std::ostream &os, const edge &e)
{
    e.print(os);
    return os;
}

std::ostream& operator<<(std::ostream &os, const figure &f)
{
    f.print(os);
    return os;
}

void figure::move(double dx, double dy)
{
    for (unsigned n = 0; n < nodes_.size(); n++) {
        node* an = get_node(n);
        an->move(dx, dy);
    }
}

};  // namespace stan

// END of this file -----------------------------------------------------------
