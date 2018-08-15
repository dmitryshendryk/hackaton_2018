cmake_minimum_required(VERSION 3.3)

project(label_maker)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "-std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "-std=c++0x")
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
#set(CMAKE_PREFIX_PATH $ENV{HOME}/Qt5.9.0/5.9/gcc_64)
set(IVISION_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/core" )
set(iVISION_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/core/build/libiVision_core.so" )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#PCL package
find_package(PCL 1.8 REQUIRED)
#Qt

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

#IVisionCore
find_package(IVisionCore REQUIRED)
#OpenCV
find_package(OpenCV 2.4.9 REQUIRED)

#Pangolin
find_package(Pangolin REQUIRED)

file(GLOB main *.cpp)
file(GLOB srcs label_maker/*.cpp)
include_directories(
${PCL_INCLUDE_DIRS}
${IVISION_INCLUDE_DIR}
${Pangolin_INCLUDE_DIRS}
${OpenCV_INCLUDE_DIRS}
)

link_directories(
  ${OPENGL_LIBRARY_DIRS}
)

add_definitions(
  ${PCL_DEFINITIONS}
)

add_executable(label_maker
  ${srcs}
  ${main}
)

target_link_libraries(label_maker
   ${iVISION_LIBRARY}
   ${Pangolin_LIBRARIES}
  ${OpenCV_LIBS}
  ${PCL_LIBRARIES}
  Qt5::Widgets
  Qt5::Core
  Qt5::Gui
)
