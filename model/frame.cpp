#include "frame.h"

namespace stan {

// static constants
const int frame::DEFAULT_WIDTH;
const int frame::DEFAULT_HEIGHT;

std::ostream& operator<<(std::ostream &os, const frame &f)
{
    f.print(os);
    return os;
}

bool frame::get_figure_at_pos(int x, int y, int radius, boost::shared_ptr<figure>& fig, boost::shared_ptr<node>& n)
{
    bool found = false;
    BOOST_FOREACH(boost::shared_ptr<figure> f, figures_)
    {
        // node positions relative to frame
        if (f->get_node_at_pos(n, x - xpos_, y - ypos_, radius))
        {
            fig = f;
            found = true;
            break;
        }
    }
    return found;
}

};  // namespace stan
