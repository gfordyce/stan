#include <iostream>
#include <fstream>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include "animation.h"

using namespace stan;

/**
 * Load the "simplefig" and verify model correctness.
 */
int main(int argc, char* argv[])
{
    animation* anim;

    if (argc < 2)
    {
        std::cerr << "Error: no filename specified." << std::endl;
        exit(-1);
    }

	std::string filename = std::string(argv[1]);
    std::cout << "Input filename is: " << filename << std::endl;

	std::ifstream ifs(filename.c_str());
	assert(ifs.good());
	{
		boost::archive::xml_iarchive ia(ifs);
        ia >> boost::serialization::make_nvp("animation", anim);

	}

    ifs.close();

    std::cout << "My animation: " << std::endl << *anim;

    // check model
    frame *fr = NULL;
    bool found = anim->get_frame_at_pos(50, 50, fr);
    assert(found);

    std::cout << "My frame: " << std::endl << *fr;
    assert(fr != NULL);

    figure* fig = NULL;
    int selected_node = -1;
    fr->get_figure_at_pos(50, 50, 10, fig, selected_node);
    assert(fig != NULL);
    assert(selected_node >= 0);

    std::cout << "Found figure " << *fig << " with selected node " << selected_node << std::endl;

    assert(fig->nodes_.size() == 4);
    for (unsigned n = 0; n < fig->nodes_.size(); n++) {
        node* pn = fig->get_node(n);
        std::cout << "Node " << n << " is at " << pn->get_x() << ", " << pn->get_y() << std::endl;
    }

    return 0;
}
