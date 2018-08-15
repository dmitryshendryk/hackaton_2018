#ifndef FORMATTER 
#define FORMATTER
#include <vector>

#include <pcl/console/parse.h>
#include <pcl/io/auto_io.h>
#include <pcl/io/obj_io.h>
#include <pcl/io/vtk_lib_io.h>

#include <boost/make_shared.hpp>

using namespace pcl;
using namespace pcl::io;
using namespace pcl::console;


namespace formatter {

    void savePLY_Cloud(const std::string &source, const std::string &dest);

}

#endif 