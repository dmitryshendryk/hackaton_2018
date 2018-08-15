cmake_minimum_required(VERSION 3.3)

project(iVision_core)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF()

MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})


if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CustomRealSenseModule.cmake")
  set(REALSENSE_CONFIGURED true)
else()
  set(REALSENSE_CONFIGURED false)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall  -O3 -march=native ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall   -O3 -march=native")
# Check C++11 or C++0x support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   add_definitions(-DCOMPILEDWITHC11)
   message(STATUS "Using flag -std=c++11.")
elseif(COMPILER_SUPPORTS_CXX0X)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
   add_definitions(-DCOMPILEDWITHC0X)
   message(STATUS "Using flag -std=c++0x.")
else()
   message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()


find_package(VTK REQUIRED)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "-std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "-std=c++0x")
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# OpenGL
find_package(OpenGL REQUIRED)
IF (OPENGL_FOUND)
  MESSAGE("-- Found OpenCV version ${OPENGL_VERSION}: ${OPENGL_INCLUDE_DIRS}")
ENDIF (OPENGL_FOUND)

set(CMAKE_INSTALL_PREFIX _install)

#GLFW
find_package(glfw3 REQUIRED)

#PCL package
find_package(PCL REQUIRED)

#Eigen
find_package( Eigen3 REQUIRED )

#GLUT
find_package(GLUT REQUIRED)

#OpenCV
find_package(OpenCV REQUIRED)



file(GLOB api ./*.cpp)
file(GLOB slam slam/src/*.cc)
file(GLOB semantic semantic/*.cpp)
file(GLOB pcl pcl/*.cpp)

add_library(${PROJECT_NAME} SHARED
  ${slam}
  ${semantic}
  ${api}
  ${pcl}
)


include_directories(
${OPENGL_INCLUDE_DIRS}
${GLFW_INCLUDE_DIR}
${EIGEN3_INCLUDE_DIR}
${GLUT_INCLUDE_DIRS}
${OpenCV_INCLUDE_DIRS}
${PROJECT_SOURCE_DIR}/slam/include
${CMAKE_CURRENT_SOURCE_DIR}/src
${Pangolin_INCLUDE_DIRS}
${PCL_INCLUDE_DIRS}
)

link_directories(
  ${OPENGL_LIBRARY_DIRS}
  ${GLFW_LIBRARY_DIRS}
)

add_definitions(
  ${OpenGL_DEFINITIONS}
  ${GLFW_DEFINITIONS}
)


target_link_libraries( ${PROJECT_NAME}
  ${PROJECT_SOURCE_DIR}/slam/Thirdparty/DBoW2/lib/libDBoW2.so
  ${PROJECT_SOURCE_DIR}/slam/Thirdparty/g2o/lib/libg2o.so
  ${Boost_FILESYSTEM_LIBRARY} 
  ${Boost_SYSTEM_LIBRARY}
  ${PCL_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${OpenCV_LIBS}
  ${GLFW_LIBRARIES}
  ${EIGEN3_LIBS}
  ${Pangolin_LIBRARIES}
  
   glfw
  -lrealsense2
)


# add_executable(rgbd_tum slam/Examples/rgbd_tum.cc)
# target_link_libraries(rgbd_tum ${PROJECT_NAME})

INSTALL(TARGETS ${PROJECT_NAME}
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
)


