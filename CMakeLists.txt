cmake_minimum_required(VERSION 2.8)
project(ecto_opencv)

find_package(catkin REQUIRED ecto opencv_candidate cv_backports)
catkin_package(DEPENDS ecto opencv_candidate cv_backports)

if(NOT CMAKE_BUILD_TYPE)#Only do this the on the first run, if the build type hasn't been set prior
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()

# manual config because of rmpm's cmake issues
# find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS}) # OpenCV_INCLUDE_DIRS needs to be is set via cmake command line option
link_directories(${OpenCV_LIB_DIRS}) # OpenCV_LIB_DIRS needs to be is set via cmake command line option
unset(OpenCV_LIBS)
unset(_tmp_abs_lib)
foreach(lib ${OpenCV_LIBRARIES})
  message(STATUS "Looking for : " ${lib})
  unset(_tmp_abs_lib CACHE)
  find_library(_tmp_abs_lib ${lib} PATHS ${OpenCV_LIB_DIRS})
  set(OpenCV_LIBS ${OpenCV_LIBS} ${_tmp_abs_lib} )
endforeach()
set(OpenCV_LIBRARIES ${OpenCV_LIBS})
message(STATUS "OpenCV_LIBS: " ${OpenCV_LIBS})
message(STATUS "OpenCV_LIBRARIES: " ${OpenCV_LIBRARIES})


message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV headers: ${OpenCV_INCLUDE_DIRS}")
include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS} ${catkin_INCLUDE_DIRS})

find_package(Boost COMPONENTS serialization filesystem system REQUIRED)

if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

# configure and install Python
catkin_python_setup()

add_subdirectory(cells)
add_subdirectory(python)

if(CATKIN_ENABLE_TESTING)
  add_subdirectory(test)
  add_subdirectory(samples)
endif()

ecto_sphinx(${CMAKE_CURRENT_SOURCE_DIR}/doc doc)
