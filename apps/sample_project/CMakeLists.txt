
add_compile_definitions(PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}/" )

add_executable(sample_project sample_project.cpp)
target_link_libraries(sample_project PUBLIC re)