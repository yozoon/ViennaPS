cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project("ViennaPS" VERSION 1.0.0)

# Store version in cache
set(VIENNAPS_VERSION
    ${PROJECT_VERSION}
    CACHE STRING "The version of ViennaPS" FORCE)

include(GNUInstallDirs)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD "17")

# Set default build type
set(DEFAULT_BUILD_TYPE "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE
      "${DEFAULT_BUILD_TYPE}"
      CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel"
                                               "RelWithDebInfo")
endif()

# Tell VS to export all symbols to its dll files
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS
      TRUE
      CACHE BOOL "Export all symbols")
  add_compile_definitions(_USE_MATH_DEFINES)
endif()

# ##################################################################################################
# CONFIGURATION OPTIONS
# ##################################################################################################
# Build the included examples
option(VIENNAPS_BUILD_EXAMPLES "Build examples." OFF)

# Build the stand-alone application
option(VIENNAPS_BUILD_APPLICATION "Build ViennaPS application." OFF)

# Whether the library should print additional messages when running
option(VIENNAPS_VERBOSE "Get verbose output and intermediate results." ON)

# Build Python bindings
option(VIENNAPS_BUILD_PYTHON "Build for python3.x." OFF)

# ##################################################################################################
# AUTOMATIC DEPENDENCY PREPARATION
# ##################################################################################################

# With the stampfile mechanism, cmake automatically retriggers the configure step after
# the buildDependencies targed has been executed. Thus all dependencies that were built
# with buildDependencies should then be made available by the find_package calls.
set(STAMPFILE ${CMAKE_BINARY_DIR}/target.stamp)
# When the buildDependencies target is executed, the stamp file is touched
add_custom_target(buildDependencies COMMAND ${CMAKE_COMMAND} -E touch ${STAMPFILE})

# Include the external dependencies
include(ExternalProject)
set(DEPENDENCIES_DIR ${CMAKE_SOURCE_DIR}/dependencies)
add_subdirectory(external/upstream)

# Create the initial stamp file
file(TOUCH ${STAMPFILE})
# Include the stamp file, so that cmake is forced to re-run once the file has been touched
include(${STAMPFILE})

set(ViennaPS_BINARY_DIR
    ${PROJECT_BINARY_DIR}
    CACHE PATH "Path to local ViennaPS installation" FORCE)

# ##################################################################################################
# DEPENDENCY CHECKS
# ##################################################################################################
set(DEPENDENCIES_FOUND TRUE)

find_package(OpenMP REQUIRED)
list(APPEND VIENNAPS_LIBRARIES OpenMP::OpenMP_CXX)

list(APPEND VIENNAPS_DEPENDENCIES "viennals_external")
find_package(ViennaLS QUIET CONFIG PATHS ${ViennaLS_DIR} NO_DEFAULT_PATH)
if(NOT ViennaLS_FOUND)
  set(DEPENDENCIES_FOUND FALSE)
endif()
list(APPEND VIENNAPS_DEPENDENCIES "viennaray_external")
find_package(ViennaRay QUIET CONFIG PATHS ${ViennaRay_DIR} NO_DEFAULT_PATH)
if(NOT ViennaRay_FOUND)
  set(DEPENDENCIES_FOUND FALSE)
endif()
if(VIENNAPS_BUILD_PYTHON)
  list(APPEND VIENNAPS_DEPENDENCIES "pybind11_external")
  find_package(pybind11 QUIET PATHS ${pybind11_DIR} NO_DEFAULT_PATH)
  if(NOT pybind11_FOUND)
    set(DEPENDENCIES_FOUND FALSE)
  endif()
endif()

if(DEPENDENCIES_FOUND)
  # Remove the buildDependencies target from the ALL target to prevent unnecessary re-builds
  set_target_properties(buildDependencies PROPERTIES EXCLUDE_FROM_ALL true)
else()
  message(WARNING "Not all dependencies were found. Execute buildDependencies target first.")
  # Add the buildDependencies target to the ALL target
  set_target_properties(buildDependencies PROPERTIES EXCLUDE_FROM_ALL false)
endif()

# ##################################################################################################
# SETUP MAIN LIBRARY TARGET
# ##################################################################################################
add_library(${PROJECT_NAME} INTERFACE)

if(VIENNAPS_VERBOSE)
  add_compile_definitions(VIENNAPS_VERBOSE)
endif()

# ##################################################################################################
# DIRECTORY CONFIGURATIONS
# ##################################################################################################

# set the correct paths for installation
set(LOCAL_CONFIG_DIR "lib/cmake/${PROJECT_NAME}")

# Adding the install interface generator expression makes sure that the include files are installed
# to the proper location (provided by GNUInstallDirs)
set(VIENNAPS_BUILD_INCLUDE_DIRS "${${PROJECT_NAME}_SOURCE_DIR}/include")
target_include_directories(
  ${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${VIENNAPS_BUILD_INCLUDE_DIRS}>
                            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

# ##################################################################################################
# CMAKE CONFIG FILE SETUP
# ##################################################################################################
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion)

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" INSTALL_DESTINATION ${LOCAL_CONFIG_DIR})

# ##################################################################################################
# BUILD EXAMPLES
# ##################################################################################################
if(VIENNAPS_BUILD_EXAMPLES AND DEPENDENCIES_FOUND)
  add_subdirectory(Examples)
endif()

# ##################################################################################################
# BUILD VIENNAPS APPLICATION
# ##################################################################################################
if(VIENNAPS_BUILD_APPLICATION AND DEPENDENCIES_FOUND)
  add_subdirectory(app)
endif()

# ##################################################################################################
# BUILD PYTHON MODULE
# ##################################################################################################
if(VIENNAPS_BUILD_PYTHON AND DEPENDENCIES_FOUND)
  add_subdirectory(Python)
endif()

# ##################################################################################################
# INSTALL
# ##################################################################################################

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}_Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(
  EXPORT ${PROJECT_NAME}_Targets
  FILE ${PROJECT_NAME}Targets.cmake
  DESTINATION ${LOCAL_CONFIG_DIR})

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${LOCAL_CONFIG_DIR})

# Install include files
file(GLOB_RECURSE HEADER_FILES "${PROJECT_SOURCE_DIR}/include/*.hpp")

install(FILES ${HEADER_FILES} DESTINATION "include")

# Uninstall target
if(NOT TARGET uninstall)
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
                 "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)

  add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P
                                      ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
