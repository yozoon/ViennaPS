# ##################################################################################################
# Check ViennaRay Dependency
# ##################################################################################################
# If the path to an installation was provided by the user
if(DEFINED ViennaRay_DIR AND NOT DEFINED CACHE{ViennaRay_DIR})
  # Required for windows to convert backslashes to forward slashes
  file(TO_CMAKE_PATH "${ViennaRay_DIR}" ViennaRay_DIR)
  set(ViennaRay_DIR
      "${ViennaRay_DIR}"
      CACHE PATH "Path to ViennaRay installation" FORCE)
  find_package(ViennaRay REQUIRED PATHS ${ViennaRay_DIR} NO_DEFAULT_PATH)
  message(STATUS "Found ViennaRay at ${ViennaRay_DIR}")
  add_library(viennaray_external INTERFACE) # dummy target
else()
  # First try to locate any local build
  find_package(ViennaRay QUIET PATHS "${DEPENDENCIES_DIR}/Install/viennaray_external"
               NO_DEFAULT_PATH)

  # If that fails look for a system installation
  if(NOT ViennaRay_FOUND)
    find_package(ViennaRay QUIET)
  endif()

  if(ViennaRay_FOUND
     AND VIENNAPS_STATIC_BUILD
     AND NOT VIENNARAY_STATIC_BUILD)
    set(ViennaRay_FOUND FALSE)
    message(
      STATUS "An installation of ViennaRay was found, but it was built without static dependencies."
    )
  endif()

  if(ViennaRay_FOUND)
    message(STATUS "Found ViennaRay at ${ViennaRay_DIR}.")
    add_library(viennaray_external INTERFACE) # dummy target
  else()
    message(
      STATUS
        "Couldn't find ViennaRay. Dependency will be built with the buildDependencies target (e.g. `make buildDependencies`)."
    )
    ExternalProject_Add(
      viennaray_external
      GIT_REPOSITORY https://github.com/ViennaTools/ViennaRay.git
      GIT_SHALLOW 1
      GIT_TAG main
      INSTALL_DIR "${DEPENDENCIES_DIR}/Install/viennaray_external"
      CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
                 -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                 -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                 -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
                 -DCMAKE_CXX_EXTENSIONS=${CMAKE_CXX_EXTENSIONS}
                 -DCMAKE_CXX_STANDARD_REQUIRED=${CMAKE_CXX_STANDARD_REQUIRED}
                 -DVIENNARAY_BUILD_EXAMPLES=OFF
                 -DVIENNARAY_BUILD_TESTS=OFF
                 -DVIENNARAY_STATIC_BUILD=${VIENNAPS_STATIC_BUILD}
                 -DVIENNARAY_DEPENDENCIES_DIR=${DEPENDENCIES_DIR}
      # Run build twice, such that the dependencies can be built if necessary
      BUILD_COMMAND ${CMAKE_COMMAND} --build . && ${CMAKE_COMMAND} --build .
      USES_TERMINAL_DOWNLOAD 1
      USES_TERMINAL_UPDATE 1
      GIT_PROGRESS 1
      EXCLUDE_FROM_ALL TRUE)

    # Get install and build directory
    ExternalProject_Get_Property(viennaray_external INSTALL_DIR)

    set(ViennaRay_DIR "${INSTALL_DIR}")

    # add to buildDependencies target
    add_dependencies(buildDependencies viennaray_external)
  endif()
  set(ViennaRay_DIR
      "${ViennaRay_DIR}"
      CACHE PATH "Path to ViennaRay installation" FORCE)
endif()
