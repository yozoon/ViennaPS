cmake_minimum_required(VERSION 3.14)

project("ProfileExtrusion")

add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${VIENNAPS_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${VIENNAPS_LIBRARIES})

configure_file(${PROJECT_NAME}.py ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.py COPYONLY)

add_dependencies(buildExamples ${PROJECT_NAME})
