#include <iostream>
#include <fstream>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include "animation.h"

using namespace stan;

/**
 * Create a figure object through deserialization.
 */
int main(int argc, char* argv[])
{
    animation* anim;

    if (argc < 2)
    {
        std::cerr << "Error: no filename specified." << std::endl;
        exit(-1);
    }

	std::string filename = std::string(argv[1]);
    std::cout << "Input filename is: " << filename << std::endl;

	std::ifstream ifs(filename.c_str());
	assert(ifs.good());
	{
		boost::archive::xml_iarchive ia(ifs);
        ia >> boost::serialization::make_nvp("animation", anim);

	}

    ifs.close();

    std::cout << "My animation: " << std::endl << *anim;
}
