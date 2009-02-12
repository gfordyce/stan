#include <iostream>
#include <fstream>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include "animation.h"

using namespace stan;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Error: no filename specified." << std::endl;
        exit(-1);
    }

	std::string filename = std::string(argv[1]);
    std::cout << "Output filename is: " << filename << std::endl;

    // create stick figure
    figure* fig(new figure(50, 50));

    int l1 = fig->create_line(fig->get_root(), 100, 50);
    int l2 = fig->create_line(fig->get_edge(l1)->get_n2(), 150, 25);
    int l3 = fig->create_line(fig->get_edge(l1)->get_n2(), 150, 75);

    // create a frame to hold our figure
    frame* fr1(new frame(0, 0, 640, 480));
    fr1->add_figure(fig);

    // create an animation
    animation* a(new animation());
    a->add_frame(fr1);

    std::cout << "My animation: " << *a << std::endl;

    // serialize it to an XML file
	std::ofstream ofs(filename.c_str());
	assert(ofs.good());
	{
		boost::archive::xml_oarchive oa(ofs);
        oa << boost::serialization::make_nvp("a", a);

	}
    ofs.close();

    // test figure copy
    figure* clone_fig(new figure(*fig));
    std::cout << "Cloned figure: " << *clone_fig << std::endl;

    return 0;
}
