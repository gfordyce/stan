#include <iostream>
#include <boost/serialization/nvp.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include "test_figure.h"

CPPUNIT_TEST_SUITE_REGISTRATION(test_figure);

void test_figure::setUp()
{
    /**
     * create stick figure
     */
    stick_fig_ = new figure(200, 140);    // root is at the  waist
    torso_ = stick_fig_->create_line(stick_fig_->get_root(), 200, 100);
    int neck = stick_fig_->create_line(stick_fig_->get_edge(torso_)->get_n2(), 200, 90);      // neck
    stick_fig_->create_circle(stick_fig_->get_edge(neck)->get_n2(), 200, 70);    // head

    int rightarm = stick_fig_->create_line(stick_fig_->get_edge(torso_)->get_n2(), 160, 120);
    stick_fig_->create_line(stick_fig_->get_edge(rightarm)->get_n2(), 140, 100);    // righthand

    int leftarm = stick_fig_->create_line(stick_fig_->get_edge(torso_)->get_n2(), 240, 120);
    stick_fig_->create_line(stick_fig_->get_edge(leftarm)->get_n2(), 260, 100);  // lefthand

    int rightthigh = stick_fig_->create_line(stick_fig_->get_root(), 180, 160);
    int rightshin = stick_fig_->create_line(stick_fig_->get_edge(rightthigh)->get_n2(), 180, 200);
    stick_fig_->create_line(stick_fig_->get_edge(rightshin)->get_n2(), 160, 200);   // rightfoot

    int leftthigh = stick_fig_->create_line(stick_fig_->get_root(), 220, 160);
    int leftshin = stick_fig_->create_line(stick_fig_->get_edge(leftthigh)->get_n2(), 220, 200);
    stick_fig_->create_line(stick_fig_->get_edge(leftshin)->get_n2(), 240, 200); // leftfoot
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
    CPPUNIT_ASSERT(neck_node->get_x() == 200);
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

    std::cout << "Serialized figure: " << *stick_fig_ << std::endl;

    /**
     * serialize it to an XML file
     */
	std::ofstream ofs(filename.c_str());
	assert(ofs.good());
	{
		ofs << "Test text for output" << std::endl;
		boost::archive::xml_oarchive oa(ofs);
        oa << boost::serialization::make_nvp("figure", *stick_fig_);

	}
    ofs.close();

#if 0
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
#endif
}

void test_figure::test_clone_subtree()
{
    figure* fig = new figure(0, 0);
    int l1 = fig->create_line(fig->get_root(), 0, 1);
    int l2 = fig->create_line(fig->get_edge(l1)->get_n2(), 1, 1);
    fig->create_line(fig->get_edge(l2)->get_n2(), 1, 2);   // l3

    std::cout << "Original figure: " << *fig << std::endl;
    int nindex = fig->get_edge(l1)->get_n2();
    std::cout << "Index of break is: " << nindex << std::endl;

    // create an empty figure
    figure* sub_fig = new figure();

    // clone the subtree into the new figure
    sub_fig->clone_subtree(fig, nindex, -1);
    
    std::cout << "Cloned subfigure: " << *sub_fig << std::endl;

    edge *e;
    node* n1, *n2;
    int l;

    // verify l2 from above was cloned
    l = sub_fig->get_edge(0, 1);
    CPPUNIT_ASSERT(l != -1);

    e = sub_fig->get_edge(l);
    CPPUNIT_ASSERT(e != NULL);

    n1 = sub_fig->get_node(e->get_n1());
    CPPUNIT_ASSERT(n1 != NULL);

    n2 = sub_fig->get_node(e->get_n2());
    CPPUNIT_ASSERT(n2 != NULL);

    CPPUNIT_ASSERT(n1->get_x() == 0);
    CPPUNIT_ASSERT(n1->get_y() == 1);
    CPPUNIT_ASSERT(n2->get_x() == 1);
    CPPUNIT_ASSERT(n2->get_y() == 1);

    // verify l3 from above was cloned
    l = sub_fig->get_edge(1, 2);
    CPPUNIT_ASSERT(l != -1);

    e = sub_fig->get_edge(l);
    CPPUNIT_ASSERT(e != NULL);

    n1 = sub_fig->get_node(e->get_n1());
    CPPUNIT_ASSERT(n1 != NULL);

    n2 = sub_fig->get_node(e->get_n2());
    CPPUNIT_ASSERT(n2 != NULL);

    CPPUNIT_ASSERT(n1->get_x() == 1);
    CPPUNIT_ASSERT(n1->get_y() == 1);
    CPPUNIT_ASSERT(n2->get_x() == 1);
    CPPUNIT_ASSERT(n2->get_y() == 2);

    // verify the correct number of edges exist in the new figure
    std::vector<edge*> edges = sub_fig->get_edges();
    CPPUNIT_ASSERT(edges.size() == 2);
}

void test_figure::test_remove_nodes()
{
    figure* fig = new figure(0, 0);
    int l1 = fig->create_line(fig->get_root(), 0, 1);
    int l2 = fig->create_line(fig->get_edge(l1)->get_n2(), 1, 1);
    fig->create_line(fig->get_edge(l2)->get_n2(), 1, 2);   // l3

    std::cout << "Original figure: " << *fig << std::endl;
    int nindex = fig->get_edge(l1)->get_n2();

    std::cout << "Index of subtree removal is: " << nindex << std::endl;

    fig->remove_nodes(nindex);

    std::cout << "Figure with nodes removed: " << *fig << std::endl;
}

// END of this file -----------------------------------------------------------
