#include <iostream>
#include <boost/serialization/nvp.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include "test_figure.h"

CPPUNIT_TEST_SUITE_REGISTRATION(test_figure);

void test_figure::setUp()
{
    // create stick figure
    stick_fig_ = new figure(100, 140);    // root is at the  waist
    torso_ = stick_fig_->create_line(stick_fig_->get_root(), 100, 100);
    int neck = stick_fig_->create_line(stick_fig_->get_edge(torso_)->get_n2(), 100, 90);      // neck
    int head = stick_fig_->create_circle(stick_fig_->get_edge(neck)->get_n2(), 100, 70);

    int rightarm = stick_fig_->create_line(stick_fig_->get_edge(torso_)->get_n2(), 60, 120);
    int righthand = stick_fig_->create_line(stick_fig_->get_edge(rightarm)->get_n2(), 40, 100);

    int leftarm = stick_fig_->create_line(stick_fig_->get_edge(torso_)->get_n2(), 140, 120);
    int lefthand = stick_fig_->create_line(stick_fig_->get_edge(leftarm)->get_n2(), 160, 100);

    int rightthigh = stick_fig_->create_line(stick_fig_->get_root(), 80, 60);
    int rightshin = stick_fig_->create_line(stick_fig_->get_edge(rightthigh)->get_n2(), 80, 200);
    int rightfoot = stick_fig_->create_line(stick_fig_->get_edge(rightshin)->get_n2(), 60, 200);

    int leftthigh = stick_fig_->create_line(stick_fig_->get_root(), 120, 60);
    int leftshin = stick_fig_->create_line(stick_fig_->get_edge(leftthigh)->get_n2(), 120, 200);
    int leftfoot = stick_fig_->create_line(stick_fig_->get_edge(leftshin)->get_n2(), 140, 200);

    // now test that get_decendants() returns the correct set of nodes
}

void test_figure::tearDown()
{
    if (stick_fig_ != NULL) {
        delete stick_fig_;
    }
}

void test_figure::test_copy()
{
    CPPUNIT_ASSERT(true);
}

void test_figure::test_get_decendants()
{
    int neck = stick_fig_->get_edge(torso_)->get_n2();
    node* neck_node = stick_fig_->get_node(neck);
    CPPUNIT_ASSERT(neck_node->get_x() == 100);
    CPPUNIT_ASSERT(neck_node->get_y() == 100);

    std::list<int> decendant_list;
    stick_fig_->get_decendants(decendant_list, neck);
    CPPUNIT_ASSERT(decendant_list.size() == 6);

    BOOST_FOREACH(int c, decendant_list) {
        std::cout << "Found decendant " << c << std::endl;
    }
}

void test_figure::test_serialization()
{
	std::string filename = "test_dude.fig";

    /**
     * create stick figure
     */
    figure* fig = new figure(200, 140);    // root is at the  waist
    int torso = fig->create_line(fig->get_root(), 200, 100);
    int neck = fig->create_line(fig->get_edge(torso)->get_n2(), 200, 90);      // neck
    int head = fig->create_circle(fig->get_edge(neck)->get_n2(), 200, 70);

    int rightarm = fig->create_line(fig->get_edge(torso)->get_n2(), 160, 120);
    int righthand = fig->create_line(fig->get_edge(rightarm)->get_n2(), 140, 100);

    int leftarm = fig->create_line(fig->get_edge(torso)->get_n2(), 240, 120);
    int lefthand = fig->create_line(fig->get_edge(leftarm)->get_n2(), 260, 100);

    int rightthigh = fig->create_line(fig->get_root(), 180, 160);
    int rightshin = fig->create_line(fig->get_edge(rightthigh)->get_n2(), 180, 200);
    int rightfoot = fig->create_line(fig->get_edge(rightshin)->get_n2(), 160, 200);

    int leftthigh = fig->create_line(fig->get_root(), 220, 160);
    int leftshin = fig->create_line(fig->get_edge(leftthigh)->get_n2(), 220, 200);
    int leftfoot = fig->create_line(fig->get_edge(leftshin)->get_n2(), 240, 200);

    std::cout << "Serialized figure: " << *fig << std::endl;

    /**
     * serialize it to an XML file
     */
	std::ofstream ofs(filename.c_str());
	assert(ofs.good());
	{
		boost::archive::xml_oarchive oa(ofs);
        oa << boost::serialization::make_nvp("figure", fig);

	}
    ofs.close();

    /**
     * Deserialize figure to a new figure.
     */
    figure* new_fig;
	std::ifstream ifs(filename.c_str());
	if (ifs.good())
	{
		boost::archive::xml_iarchive ia(ifs);
        ia >> boost::serialization::make_nvp("figure", new_fig);
        if (new_fig == NULL) {
            std::cerr << "Error loading " << filename << std::endl;
        }
	}
    ifs.close();

    std::cout << "Deserialized figure: " << *new_fig << std::endl;
}
// END of this file -----------------------------------------------------------
