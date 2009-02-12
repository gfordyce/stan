#include <iostream>
#include "animation.h"
#include "trig.h"

using namespace stan;

int main(int argc, char* argv[])
{
    // create stick figure
    figure* sel_fig(new figure(0, 0));

    int l1 = sel_fig->create_line(sel_fig->get_root(), 10, 10);

    // test figure copy
    figure* pivot_fig(new figure(*sel_fig));

    std::cout << "Original figure: " << *pivot_fig << std::endl;
    std::cout << "Cloned figure: " << *pivot_fig << std::endl;

    edge* e = sel_fig->get_edge(l1);

    int selected = e->get_n2();
    int pivot = e->get_n1();

    std::list<int> pivot_nodes;
    pivot_nodes.push_back(selected);
    pivot_nodes.push_back(pivot);

    // now let's rotate through pieces of pie...mmm
    int num_slices = 4;
    double slice_width = (2 * PI) / num_slices;
    for (int pie_slice = 0; pie_slice <= num_slices; pie_slice++) {
        double angle = pie_slice * slice_width;
        std::cout << "Rotating by " << rad2deg(angle) << std::endl;

        // rotate
        rotate_figure(sel_fig, pivot_fig, pivot, pivot_nodes, angle);
        std::cout << "Rotated figure: " << *pivot_fig << std::endl;
    }
    return 0;
}
