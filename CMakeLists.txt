# Set the project name
project(STAN)

# Require at least CMake version 2.6
cmake_minimum_required(VERSION 2.6)

# Supress automatic regeneration of project files
set(CMAKE_SUPPRESS_REGENERATION TRUE)

# Add executable's directory to its library search path
set(CMAKE_INSTALL_RPATH ".")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

# Find third-party packages
find_package(Boost REQUIRED serialization thread)
#find_package(PythonLibs REQUIRED)
find_package(wxWidgets REQUIRED base core net adv)

# On Windows the Boost libraries are linked automatically
if(MSVC)
    set(Boost_LIBRARIES "")
endif()

# Set compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX /wd4251 /wd4275 /wd4996")
else()
    add_definitions(-D_LINUX)
endif()

# Add STAN components
add_subdirectory(controller)

# Macro for embedding resources
#macro(add_resources)
#    execute_process(COMMAND python ${ISTT_SOURCE_DIR}/CMakeRes.py
#        ${CMAKE_CURRENT_SOURCE_DIR}/Resources ${CMAKE_CURRENT_BINARY_DIR}
#        RESULT_VARIABLE Result ERROR_VARIABLE Error)
#    if(NOT Result EQUAL 0)
#        message("Resource script had error: ${Error}")
#    endif()
#    include_directories(${CMAKE_CURRENT_BINARY_DIR})
#    set(HeaderFiles ${HeaderFiles} Resources.hpp)
#    set(SourceFiles ${SourceFiles} Resources.cpp)
#endmacro()

#enable_testing()
#INCLUDE(Dart)
#add_test(first ~/dev/build/test/first)
#add_test(figtest ~/dev/build/test/figtest anim.xml)
#add_test(loadfig ~/dev/build/test/loadfig anim.xml)
#add_test(simplefig ~/dev/build/test/simplefig simple.xml)
#add_test(simplecheck ~/dev/build/test/simplecheck simple.xml)
#add_test(rotfig ~/dev/build/test/rotfig)
#add_test(test_runner ~/dev/build/test/test_runner)
