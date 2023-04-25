cmake_minimum_required(VERSION 3.12)

project(ViennaPSPython)

include("${CMAKE_SOURCE_DIR}/cmake/prepare.cmake")

# Point find_package to the binary directory instead of the install location
find_package(ViennaPS REQUIRED PATHS ${ViennaPS_BINARY_DIR} NO_DEFAULT_PATH)

list(
  PREPEND
  VIENNAPS_INCLUDE_DIRS
  ${VIENNAPS_BUILD_INCLUDE_DIRS}
  ${VIENNAPS_BUILD_INCLUDE_DIRS}/CellSet
  ${VIENNAPS_BUILD_INCLUDE_DIRS}/Geometries
  ${VIENNAPS_BUILD_INCLUDE_DIRS}/Models
  ${VIENNAPS_BUILD_INCLUDE_DIRS}/Compact)

message(STATUS "ViennaPS version: ${VIENNAPS_VERSION}")

set(VIENNAPS_PYTHON_SOURCE pyWrap.cpp)
set(VIENNAPS_PYTHON_MODULE_NAME "viennaps")

set(PYBIND11_PYTHON_VERSION
    3
    CACHE STRING "Python version")

find_package(pybind11 REQUIRED PATHS ${pybind11_DIR})

# ##################################################################################################
# BUILD 2D PYTHON LIBRARIES
# ##################################################################################################
set(VIENNAPS_PYTHON_MODULE_NAME_2D "_${VIENNAPS_PYTHON_MODULE_NAME}2d")
pybind11_add_module(${VIENNAPS_PYTHON_MODULE_NAME_2D} ${VIENNAPS_PYTHON_SOURCE})
target_include_directories(${VIENNAPS_PYTHON_MODULE_NAME_2D} PUBLIC ${VIENNAPS_INCLUDE_DIRS})
target_link_libraries(${VIENNAPS_PYTHON_MODULE_NAME_2D} PRIVATE ${VIENNAPS_LIBRARIES})
target_compile_definitions(
  ${VIENNAPS_PYTHON_MODULE_NAME_2D}
  PRIVATE -DVIENNAPS_PYTHON_DIMENSION=2 -DVIENNAPS_MODULE_NAME=${VIENNAPS_PYTHON_MODULE_NAME_2D})

if(DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(MODULE_NAME ${VIENNAPS_PYTHON_MODULE_NAME_2D})
  set_target_properties(
    ${VIENNAPS_PYTHON_MODULE_NAME_2D} PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                                                 ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/viennaps2d)
  configure_file(__init__.py.in ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/viennaps2d/__init__.py)
endif()

# ##################################################################################################
# BUILD 3D PYTHON LIBRARIES
# ##################################################################################################
set(VIENNAPS_PYTHON_MODULE_NAME_3D "_${VIENNAPS_PYTHON_MODULE_NAME}3d")
pybind11_add_module(${VIENNAPS_PYTHON_MODULE_NAME_3D} ${VIENNAPS_PYTHON_SOURCE})
target_include_directories(${VIENNAPS_PYTHON_MODULE_NAME_3D} PUBLIC ${VIENNAPS_INCLUDE_DIRS})
target_link_libraries(${VIENNAPS_PYTHON_MODULE_NAME_3D} PRIVATE ${VIENNAPS_LIBRARIES})
target_compile_definitions(
  ${VIENNAPS_PYTHON_MODULE_NAME_3D}
  PRIVATE -DVIENNAPS_PYTHON_DIMENSION=3 -DVIENNAPS_MODULE_NAME=${VIENNAPS_PYTHON_MODULE_NAME_3D})

if(DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(MODULE_NAME ${VIENNAPS_PYTHON_MODULE_NAME_3D})
  set_target_properties(
    ${VIENNAPS_PYTHON_MODULE_NAME_3D} PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                                                 ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/viennaps3d)
  configure_file(__init__.py.in ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/viennaps3d/__init__.py)
endif()
