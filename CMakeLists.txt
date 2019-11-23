cmake_minimum_required(VERSION 3.3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_SHARED_LIBS OFF)
add_compile_definitions(GLEW_STATIC)
add_library(re INTERFACE)

add_subdirectory(${CMAKE_SOURCE_DIR}/third_party)
add_subdirectory(${CMAKE_SOURCE_DIR}/renderer)
add_subdirectory(${CMAKE_SOURCE_DIR}/app)

target_link_libraries(re INTERFACE 
    core
    glm
    stbimage
    glfw
    glad
    imgui
)
