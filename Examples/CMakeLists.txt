cmake_minimum_required(VERSION 3.12)

project(ViennaPSExamples)

include("${CMAKE_SOURCE_DIR}/cmake/prepare.cmake")

find_package(ViennaPS REQUIRED PATHS ${ViennaPS_BINARY_DIR} NO_DEFAULT_PATH)

list(PREPEND VIENNAPS_INCLUDE_DIRS ${VIENNAPS_BUILD_INCLUDE_DIRS})

# Add subfolders to the include directories, since during installation the directory structure is
# flattened, but in the source tree - which we are using here - it is not.
list(PREPEND VIENNAPS_INCLUDE_DIRS ${VIENNAPS_BUILD_INCLUDE_DIRS}/CellSet
     ${VIENNAPS_BUILD_INCLUDE_DIRS}/Geometries ${VIENNAPS_BUILD_INCLUDE_DIRS}/Models
     ${VIENNAPS_BUILD_INCLUDE_DIRS}/Compact)

# Exclude the buildExamples target from the `all` target
set_property(DIRECTORY PROPERTY EXCLUDE_FROM_ALL TRUE)

add_custom_target(buildExamples)

subdirlist(EXAMPLE_DIRS ${CMAKE_CURRENT_LIST_DIR})

# add all examples
foreach(subdir ${EXAMPLE_DIRS})
  add_subdirectory(${subdir})
endforeach()
