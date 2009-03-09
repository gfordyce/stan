#ifndef _TEST_FRAME_H
#define _TEST_FRAME_H      1

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "frame.h"

using namespace stan;

class test_frame : public CppUnit::TestFixture
{
    private:
        CPPUNIT_TEST_SUITE(test_frame);
        CPPUNIT_TEST(test_copy);
        CPPUNIT_TEST(test_serialization);
        CPPUNIT_TEST(test_iterator);
        CPPUNIT_TEST_SUITE_END ();

    public:
        test_frame()
        {}

        void setUp();
        void tearDown();

    protected:
        /**
         * Test that the copy constructor copies a frame correctly.
         */
        void test_copy();

        /**
         * Test proper serialization / deserialization of a frame object.
         */
        void test_serialization();

        void test_iterator();

    private:
        frame* test_fr_;
};

#endif  // _TEST_FRAME
