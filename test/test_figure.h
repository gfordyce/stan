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
        CPPUNIT_TEST_SUITE_END ();

    public:
        test_figure() :
            stick_fig_(NULL)
        {}

        void setUp();
        void tearDown();

    protected:
        void test_copy();
        void test_get_decendants();

    private:
        figure* stick_fig_;
        int torso_;
};

#endif  // _TEST_FIGURE
