cmake_minimum_required(VERSION 3.3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")

add_compile_definitions(USE_PHYSX)

add_compile_definitions(GLEW_STATIC)
add_library(re INTERFACE)

add_subdirectory(${CMAKE_SOURCE_DIR}/third_party)
add_subdirectory(${CMAKE_SOURCE_DIR}/engine)
add_subdirectory(${CMAKE_SOURCE_DIR}/apps)

target_link_libraries(re INTERFACE 
    core
    glm
    stbimage
    glfw
    glad
    imgui
    tinyobjloader
    bullet
    physx
)
