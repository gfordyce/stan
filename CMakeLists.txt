cmake_minimum_required(VERSION 2.6)

project(stan)

set (BOOST_ROOT c:/dev/3p/boost/boost_1_38)
set (CPPUNIT_ROOT c:/dev/3p/cppunit-1.12.1)
set (STAN_DIRS model view controller utils test)

find_package(wxWidgets REQUIRED base core)
include(${wxWidgets_USE_FILE})

include_directories(. model view controller utils ${BOOST_ROOT} ${CPPUNIT_ROOT}/include)
link_directories(${BOOST_ROOT}/lib ${CPPUNIT_ROOT}/lib)

foreach(dir ${STAN_DIRS})
    add_subdirectory(${dir})
endforeach(dir)

#enable_testing()
#INCLUDE(Dart)
#add_test(first ~/dev/build/test/first)
#add_test(figtest ~/dev/build/test/figtest anim.xml)
#add_test(loadfig ~/dev/build/test/loadfig anim.xml)
#add_test(simplefig ~/dev/build/test/simplefig simple.xml)
#add_test(simplecheck ~/dev/build/test/simplecheck simple.xml)
#add_test(rotfig ~/dev/build/test/rotfig)
#add_test(test_runner ~/dev/build/test/test_runner)
