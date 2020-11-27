### GLM
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE glm)

### TINYOBJLOADER
add_library(tinyobjloader INTERFACE)
target_include_directories(tinyobjloader INTERFACE tinyobjloader)

### STBIMAGE
add_library(stbimage INTERFACE)
target_include_directories(stbimage INTERFACE stbimage)

### GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_VULKAN_STATIC OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

### GLAD
add_library(glad glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

### DEAR IMGUI
add_subdirectory(imgui)
target_include_directories(imgui PUBLIC imgui/include)

### PHYSX
add_library(physx INTERFACE)
target_include_directories(physx INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/physx/include")
target_include_directories(physx INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/physx/include/physx")
set(PHYSX_LIBS_DEBUG 
    "${CMAKE_CURRENT_SOURCE_DIR}/physx/lib/debug/PhysX_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/physx/lib/debug/PhysXCommon_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/physx/lib/debug/PhysXCooking_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/physx/lib/debug/PhysXExtensions_static_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/physx/lib/debug/PhysXFoundation_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/physx/lib/debug/PhysXPvdSDK_static_64.lib"
)
set(PHYSX_LIBS_RELEASE 
    "${CMAKE_CURRENT_SOURCE_DIR}/physx/lib/release/PhysX_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/physx/lib/release/PhysXCommon_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/physx/lib/release/PhysXCooking_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/physx/lib/release/PhysXExtensions_static_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/physx/lib/release/PhysXFoundation_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/physx/lib/release/PhysXPvdSDK_static_64.lib"
)


if (NOT DEFINED CMAKE_BUILD_TYPE)
    message(FATAL_ERROR "CMAKE_BUILD_TYPE undefined!")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Using release")
    target_link_libraries(physx INTERFACE ${PHYSX_LIBS_RELEASE}) 
else()
    message("Using debug")
    target_link_libraries(physx INTERFACE ${PHYSX_LIBS_DEBUG}) 
endif ()