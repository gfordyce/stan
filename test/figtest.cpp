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

#if 0
    // create stick figure
    figure* fig(new figure(60, 200));    // root is at the  waist

    //node* n1 = fig->create_node(60, 200);
    int  l1 = fig->create_line(fig->get_root(), 80, 200);
    int l2 = fig->create_line(fig->get_edge(l1)->get_n2(), 80, 160);
    int l3 = fig->create_line(fig->get_edge(l2)->get_n2(), 100, 140);   // the waist
    int l4 = fig->create_line(fig->get_edge(l3)->get_n2(), 120, 160);
    int l5 = fig->create_line(fig->get_edge(l4)->get_n2(), 120, 200);
    int l6 = fig->create_line(fig->get_edge(l5)->get_n2(), 140, 200);
    int l7 = fig->create_line(fig->get_edge(l3)->get_n2(), 100, 100);   // the neck
    int l8 = fig->create_line(fig->get_edge(l7)->get_n2(), 60, 120);
    int l9 = fig->create_line(fig->get_edge(l8)->get_n2(), 40, 100);
    int l10 = fig->create_line(fig->get_edge(l7)->get_n2(), 140, 120);
    int l11 = fig->create_line(fig->get_edge(l10)->get_n2(), 160, 100);
    int l12 = fig->create_line(fig->get_edge(l7)->get_n2(), 100, 90);
    int c1 = fig->create_circle(fig->get_edge(l12)->get_n2(), 100, 70);
#endif

    // create stick figure
    figure* fig = new figure(100, 140);    // root is at the  waist
    int torso = fig->create_line(fig->get_root(), 100, 100);
    int neck = fig->create_line(fig->get_edge(torso)->get_n2(), 100, 90);      // neck
    int head = fig->create_circle(fig->get_edge(neck)->get_n2(), 100, 70);

    int rightarm = fig->create_line(fig->get_edge(torso)->get_n2(), 60, 120);
    int righthand = fig->create_line(fig->get_edge(rightarm)->get_n2(), 40, 100);

    int leftarm = fig->create_line(fig->get_edge(torso)->get_n2(), 140, 120);
    int lefthand = fig->create_line(fig->get_edge(leftarm)->get_n2(), 160, 100);

    int rightthigh = fig->create_line(fig->get_root(), 80, 160);
    int rightshin = fig->create_line(fig->get_edge(rightthigh)->get_n2(), 80, 200);
    int rightfoot = fig->create_line(fig->get_edge(rightshin)->get_n2(), 60, 200);

    int leftthigh = fig->create_line(fig->get_root(), 120, 160);
    int leftshin = fig->create_line(fig->get_edge(leftthigh)->get_n2(), 120, 200);
    int leftfoot = fig->create_line(fig->get_edge(leftshin)->get_n2(), 140, 200);

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
