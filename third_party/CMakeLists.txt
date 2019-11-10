
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE glm)

add_library(stbimage INTERFACE)
target_include_directories(stbimage INTERFACE stbimage)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_VULKAN_STATIC OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)


add_library(glad glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)