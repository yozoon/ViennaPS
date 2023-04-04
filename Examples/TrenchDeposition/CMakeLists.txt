cmake_minimum_required(VERSION 3.12)

project("TrenchDeposition")

add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${VIENNAPS_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${VIENNAPS_LIBRARIES})

configure_file(TrenchDeposition.py ${CMAKE_CURRENT_BINARY_DIR}/TrenchDeposition.py COPYONLY)

add_dependencies(buildExamples ${PROJECT_NAME})
