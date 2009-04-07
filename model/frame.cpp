#include "frame.h"

namespace stan {

// static constants
//const int frame::DEFAULT_WIDTH;
//const int frame::DEFAULT_HEIGHT;

std::ostream& operator<<(std::ostream &os, const frame &f)
{
    f.print(os);
    return os;
}

bool frame::get_figure_at_pos(int x, int y, int radius, figure*& fig, int& n)
{
    bool found = false;

    BOOST_FOREACH(figure* f, figures_) {
        // node positions relative to frame
        if (f->get_node_at_pos(n, x - xpos_, y - ypos_, radius)) {
            fig = f;
            found = true;
            break;
        }
    }

    return found;
}

void frame::break_figure(figure* fig, int nindex)
{
    std::cout << "frame::break_figure." << std::endl;

    // construct a new figure out of list and root with given position offset
    figure* nfig = new figure();
    nfig->clone_subtree(fig, nindex, -1);
    figures_.push_back(nfig);
    nfig->move(20, 20); // offset the new figure so we can see it

    // remove decendant nodes from original figure
    // TODO: fig->remove_nodes(nindex);
}

};  // namespace stan
