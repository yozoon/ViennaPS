cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(ViennaPSPython)

include("${CMAKE_SOURCE_DIR}/cmake/prepare.cmake")

# Point find_package to the binary directory instead of the install location
find_package(ViennaPS CONFIG PATHS ${ViennaPS_BINARY_DIR} NO_DEFAULT_PATH)
if(NOT ViennaPS_FOUND)
  message(
    FATAL_ERROR
      "One of the dependencies hasn't been built yet and thus can't be used by the buildTests target.\n\
Try the following steps to fix the problem:\n\
1. Turn off tests by disabling the VIENNAPS_BUILD_TESTS option when configuring the project\n\
   (e.g. with the `-D VIENNAPS_BUILD_TESTS=OFF` flag)\n\
2. Build the dependencies target: e.g. `make buildDependencies` when using Makefiles\n\
3. Turn on tests again by re-enabling the VIENNAPS_BUILD_TESTS option\n\
   (e.g. with the `-D VIENNAPS_BUILD_TESTS=ON` flag)\n\
4. Now the configure step should pick up the built dependencies and proceed as intended.")
endif()

list(PREPEND VIENNAPS_INCLUDE_DIRS ${VIENNAPS_BUILD_INCLUDE_DIRS}/CellSet
     ${VIENNAPS_BUILD_INCLUDE_DIRS}/Geometries ${VIENNAPS_BUILD_INCLUDE_DIRS}/Models
     ${VIENNAPS_BUILD_INCLUDE_DIRS}/Compact)

message(STATUS "ViennaPS version: ${VIENNAPS_VERSION}")

set(VIENNAPS_PYTHON_SOURCE pyWrap.cpp)
set(VIENNAPS_PYTHON_MODULE_NAME "viennaps")

set(PYBIND11_PYTHON_VERSION
    3
    CACHE STRING "Python version")

find_package(pybind11 REQUIRED PATHS ${pybind11_DIR})

# make 3.6 python module define dimension for build
set(VIENNAPS_PYTHON_MODULE_NAME_2D "${VIENNAPS_PYTHON_MODULE_NAME}2d")

pybind11_add_module(${VIENNAPS_PYTHON_MODULE_NAME_2D} ${VIENNAPS_PYTHON_SOURCE})
target_include_directories(${VIENNAPS_PYTHON_MODULE_NAME_2D} PUBLIC ${VIENNAPS_INCLUDE_DIRS})
target_link_libraries(${VIENNAPS_PYTHON_MODULE_NAME_2D} PRIVATE ${VIENNAPS_LIBRARIES})
# define compile dimension
target_compile_definitions(${VIENNAPS_PYTHON_MODULE_NAME_2D}
                            PRIVATE -DVIENNAPS_PYTHON_DIMENSION=2)

# define dimension for build
set(VIENNAPS_PYTHON_MODULE_NAME_3D "${VIENNAPS_PYTHON_MODULE_NAME}3d")

pybind11_add_module(${VIENNAPS_PYTHON_MODULE_NAME_3D} ${VIENNAPS_PYTHON_SOURCE})
target_include_directories(${VIENNAPS_PYTHON_MODULE_NAME_3D} PUBLIC ${VIENNAPS_INCLUDE_DIRS})
target_link_libraries(${VIENNAPS_PYTHON_MODULE_NAME_3D} PRIVATE ${VIENNAPS_LIBRARIES})
# define compile dimension
target_compile_definitions(${VIENNAPS_PYTHON_MODULE_NAME_3D}
                            PRIVATE -DVIENNAPS_PYTHON_DIMENSION=3)
