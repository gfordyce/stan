#include <iostream>
#include <fstream>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include "animation.h"

using namespace stan;

/**
 * Figures are composed of shapes. Shapes are anchored by "nodes".
 */

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
    figure* fig(new figure(60, 200));    // root is at the  waist

    //node* n1 = fig->create_node(60, 200);
    edge* l1 = fig->create_line(fig->get_root(), 80, 200);
    edge* l2 = fig->create_line(l1->get_n2(), 80, 160);
    edge* l3 = fig->create_line(l2->get_n2(), 100, 140);   // the waist
    edge* l4 = fig->create_line(l3->get_n2(), 120, 160);
    edge* l5 = fig->create_line(l4->get_n2(), 120, 200);
    edge* l6 = fig->create_line(l5->get_n2(), 140, 200);
    edge* l7 = fig->create_line(l3->get_n2(), 100, 100);   // the neck
    edge* l8 = fig->create_line(l7->get_n2(), 60, 120);
    edge* l9 = fig->create_line(l8->get_n2(), 40, 100);
    edge* l10 = fig->create_line(l7->get_n2(), 140, 120);
    edge* l11 = fig->create_line(l10->get_n2(), 160, 100);
    edge* l12 = fig->create_line(l7->get_n2(), 100, 90);
    edge* c1 = fig->create_circle(l12->get_n2(), 100, 70);

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
