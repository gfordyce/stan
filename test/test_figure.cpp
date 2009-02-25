#include <iostream>
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
    CPPUNIT_ASSERT(decendant_list.size() == 5);

    BOOST_FOREACH(int c, decendant_list) {
        std::cout << "Found decendant " << c << std::endl;
    }
}

// END of this file -----------------------------------------------------------
