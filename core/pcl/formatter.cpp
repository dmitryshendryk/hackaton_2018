#include "formatter.hpp"

#define ASCII 0
#define BINARY 1
#define BINARY_COMPRESSED 2


bool
saveMesh (pcl::PolygonMesh& input,
          std::string output_file,
          int output_type);

/**
 * Saves a cloud into the specified file and output type. The file format is automatically parsed.
 * @param input[in] The cloud to be saved
 * @param output_file[out] The output file to be written
 * @param output_type[in] The output file type
 * @return True on success, false otherwise.
 */
bool
savePointCloud (pcl::PCLPointCloud2::Ptr input,
                std::string output_file,
                int output_type)
{
  if (boost::filesystem::path (output_file).extension () == ".pcd")
  {
    //TODO Support precision, origin, orientation
    pcl::PCDWriter w;
    if (output_type == ASCII)
    {
      PCL_INFO ("Saving file %s as ASCII.\n", output_file.c_str ());
      if (w.writeASCII (output_file, *input) != 0)
        return (false);
    }
    else if (output_type == BINARY)
    {
      PCL_INFO ("Saving file %s as binary.\n", output_file.c_str ());
      if (w.writeBinary (output_file, *input) != 0)
        return (false);
    }
    else if (output_type == BINARY_COMPRESSED)
    {
      PCL_INFO ("Saving file %s as binary compressed.\n", output_file.c_str ());
      if (w.writeBinaryCompressed (output_file, *input) != 0)
        return (false);
    }
  }
  else if (boost::filesystem::path (output_file).extension () == ".stl")
  {
    PCL_ERROR ("STL file format does not support point clouds! Aborting.\n");
    return (false);
  }
  else  // OBJ, PLY and VTK
  {
    //TODO: Support precision
    //FIXME: Color is lost during OBJ conversion (OBJ supports color)
    pcl::PolygonMesh mesh;
    mesh.cloud = *input;
    if (!saveMesh (mesh, output_file, output_type))
      return (false);
  }

  return (true);
}

/**
 * Saves a mesh into the specified file and output type. The file format is automatically parsed.
 * @param input[in] The mesh to be saved
 * @param output_file[out] The output file to be written
 * @param output_type[in]  The output file type
 * @return True on success, false otherwise.
 */
bool
saveMesh (pcl::PolygonMesh& input,
          std::string output_file,
          int output_type)
{
  if (boost::filesystem::path (output_file).extension () == ".obj")
  {
    if (output_type == BINARY || output_type == BINARY_COMPRESSED)
      PCL_WARN ("OBJ file format only supports ASCII.\n");

    //TODO: Support precision
    //FIXME: Color is lost during conversion (OBJ supports color)
    PCL_INFO ("Saving file %s as ASCII.\n", output_file.c_str ());
    if (pcl::io::saveOBJFile (output_file, input) != 0)
      return (false);
  }
  else if (boost::filesystem::path (output_file).extension () == ".pcd")
  {
    if (!input.polygons.empty ())
      PCL_WARN ("PCD file format does not support meshes! Only points be saved.\n");
    pcl::PCLPointCloud2::Ptr cloud = boost::make_shared<pcl::PCLPointCloud2> (input.cloud);
    if (!savePointCloud (cloud, output_file, output_type))
      return (false);
  }
  else  // PLY, STL and VTK
  {
    if (output_type == BINARY_COMPRESSED)
      PCL_WARN ("PLY, STL and VTK file formats only supports ASCII and binary output file types.\n");

    if (input.polygons.empty() && boost::filesystem::path (output_file).extension () == ".stl")
    {
      PCL_ERROR ("STL file format does not support point clouds! Aborting.\n");
      return (false);
    }

    PCL_INFO ("Saving file %s as %s.\n", output_file.c_str (), (output_type == ASCII) ? "ASCII" : "binary");
    if (!pcl::io::savePolygonFile (output_file, input, (output_type == ASCII) ? false : true))
      return (false);
  }

  return (true);
}

void formatter::savePLY_Cloud(const std::string &source, const std::string &dest)
{
  bool cloud_output (false);
  int output_type (BINARY);
  // Try to load as mesh
  pcl::PolygonMesh mesh;
  if (boost::filesystem::path (source).extension () != ".pcd" &&
      pcl::io::loadPolygonFile (source, mesh) != 0)
  {
    PCL_INFO ("Loaded a mesh with %d points (total size is %d) and the following channels:\n%s\n",
             mesh.cloud.width * mesh.cloud.height, mesh.cloud.data.size (), pcl::getFieldsList (mesh.cloud).c_str ());

    if (cloud_output)
      mesh.polygons.clear();

    if (!saveMesh (mesh, dest, output_type))
        return ;
  }
  else if (boost::filesystem::path (source).extension () == ".stl")
  {
    PCL_ERROR ("Unable to load %s.\n", source);
  }
  else
  {
    // PCD, OBJ, PLY or VTK
    if (boost::filesystem::path (source).extension () != ".pcd")
      PCL_WARN ("Could not load %s as a mesh, trying as a point cloud instead.\n", source);

    //Eigen::Vector4f origin; // TODO: Support origin/orientation
    //Eigen::Quaternionf orientation;
    pcl::PCLPointCloud2::Ptr cloud (new pcl::PCLPointCloud2);
    if (pcl::io::load (source, *cloud) < 0)
    {
      PCL_ERROR ("Unable to load %s.\n", source);
    }

    PCL_INFO ("Loaded a point cloud with %d points (total size is %d) and the following channels:\n%s\n", cloud->width * cloud->height, cloud->data.size (),
              pcl::getFieldsList (*cloud).c_str ());

    if (!savePointCloud (cloud, dest, output_type))
    {
      PCL_ERROR ("Failed to save %s.\n", dest);
    }
  }
}