#include <iostream>
#include <boost/serialization/nvp.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include "test_frame.h"

CPPUNIT_TEST_SUITE_REGISTRATION(test_frame);

void test_frame::setUp()
{
    // create a simple stick figure
    figure* fig(new figure(50, 50));

    int l1 = fig->create_line(fig->get_root(), 100, 50);
    int l2 = fig->create_line(fig->get_edge(l1)->get_n2(), 150, 25);
    int l3 = fig->create_line(fig->get_edge(l1)->get_n2(), 150, 75);

    // create a frame to hold our figure
    test_fr_ = new frame(0, 0, 640, 480);
    test_fr_->add_figure(fig);
}

void test_frame::tearDown()
{
    if (test_fr_ != NULL) {
        delete test_fr_;
    }
}

void test_frame::test_copy()
{
    frame* new_fr = new frame(*test_fr_);
    std::cout << "Original frame: " << *test_fr_ << std::endl;
    std::cout << "New frame: " << *new_fr << std::endl;
}

void test_frame::test_serialization()
{
	std::string filename = "test_frame.frm";

    /**
     * serialize frame to a FRM file
     */
	std::ofstream ofs(filename.c_str());
	assert(ofs.good());
	{
		boost::archive::xml_oarchive oa(ofs);
        oa << boost::serialization::make_nvp("frame", test_fr_);

	}
    ofs.close();

    /**
     * Deserialize to a new frame.
     */
    frame* new_frame;
	std::ifstream ifs(filename.c_str());
	if (ifs.good())
	{
		boost::archive::xml_iarchive ia(ifs);
        ia >> boost::serialization::make_nvp("frame", new_frame);
        if (new_frame == NULL) {
            std::cerr << "Error loading " << filename << std::endl;
        }
	}
    ifs.close();

    std::cout << "Deserialized frame: " << *new_frame << std::endl;
}

void test_frame::test_iterator()
{
    // answer some questions about STL iterators and lists
    std::list<int> ilist;
    for (int i = 3; i < 8; i++) {
        ilist.push_back(i);
    }

    std::list<int>::iterator iter = std::find(ilist.begin(), ilist.end(), 3);
    CPPUNIT_ASSERT(*iter == 3);
    CPPUNIT_ASSERT(iter == ilist.begin());
    iter--;
    CPPUNIT_ASSERT(iter != ilist.begin());

    iter = std::find(ilist.begin(), ilist.end(), 7);
    CPPUNIT_ASSERT(*iter == 7);
    CPPUNIT_ASSERT(iter != ilist.end());
    iter++;
    CPPUNIT_ASSERT(iter == ilist.end());
}

// END of this file -----------------------------------------------------------
