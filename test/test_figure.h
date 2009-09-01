#ifndef _TEST_FIGURE_H
#define _TEST_FIGURE_H      1

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "figure.h"

using namespace stan;

class test_figure : public CppUnit::TestFixture
{
    private:
        CPPUNIT_TEST_SUITE(test_figure);
        CPPUNIT_TEST(test_copy);
        CPPUNIT_TEST(test_get_decendants);
        CPPUNIT_TEST(test_serialization);
        CPPUNIT_TEST(test_clone_subtree);
        CPPUNIT_TEST(test_remove_nodes);
        CPPUNIT_TEST(test_image_store);
        CPPUNIT_TEST_SUITE_END ();

    public:
        test_figure() :
            stick_fig_(NULL)
        {}

        void setUp();
        void tearDown();

    protected:
        /**
         * Test that the copy constructor copies a figure correctly.
         */
        void test_copy();

        /**
         * Test that get_decendants creates proper node set.
         */
        void test_get_decendants();

        /**
         * Test proper serialization / deserialization of a figure object.
         */
        void test_serialization();

        /**
         * Test ability to clone a subtree from an existing figure.
         */
        void test_clone_subtree();

        /**
         * Test ability to remove nodes (subtrees) from the figure.
         */
        void test_remove_nodes();

        /**
         * Test the image storage classes.
         */
        void test_image_store();

    private:
        figure* stick_fig_;
        int torso_;
        int rightarm_;
};

#endif  // _TEST_FIGURE
