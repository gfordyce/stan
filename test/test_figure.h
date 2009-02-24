#ifndef _TEST_FIGURE_H
#define _TEST_FIGURE_H      1

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class test_figure : public CppUnit::TestFixture
{
    private:
        CPPUNIT_TEST_SUITE(test_figure);
        CPPUNIT_TEST(test_copy);
        CPPUNIT_TEST_SUITE_END ();

    public:
        void setUp();
        void tearDown();

    protected:
        void test_copy();
};

#endif  // _TEST_FIGURE
