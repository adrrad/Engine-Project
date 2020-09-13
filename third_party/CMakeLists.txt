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

### BULLET
add_library(bullet INTERFACE)
target_include_directories(bullet INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/bullet/include")

set(BULLET_LIBS_DEBUG 
    "${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/debug/BulletCollision_Debug.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/debug/BulletDynamics_Debug.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/debug/LinearMath_Debug.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/debug/BulletSoftBody_Debug.lib"
)
set(BULLET_LIBS_RELEASE 
    "${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/release/BulletCollision.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/release/BulletDynamics.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/release/LinearMath.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/release/BulletSoftBody.lib"
)

# https://github.com/microsoft/vscode-cmake-tools/issues/1358 - How to fix missing CMAKE_BUILD_TYPE
message(${CMAKE_BUILD_TYPE})
if (NOT DEFINED CMAKE_BUILD_TYPE)
    message(FATAL_ERROR "CMAKE_BUILD_TYPE undefined!")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Using release")
    target_link_libraries(bullet INTERFACE ${BULLET_LIBS_RELEASE}) 
else()
    message("Using debug")
    target_link_libraries(bullet INTERFACE ${BULLET_LIBS_DEBUG}) 
endif ()


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
    "${CMAKE_CURRENT_SOURCE_DIR}/physc/lib/release/PhysX_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/physc/lib/release/PhysXCommon_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/physc/lib/release/PhysXCooking_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/physx/lib/release/PhysXExtensions_static_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/physc/lib/release/PhysXFoundation_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/physx/lib/debug/PhysXPvdSDK_static_64.lib"
)

message(${CMAKE_BUILD_TYPE})
if (NOT DEFINED CMAKE_BUILD_TYPE)
    message(FATAL_ERROR "CMAKE_BUILD_TYPE undefined!")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Using release")
    target_link_libraries(physx INTERFACE ${PHYSX_LIBS_RELEASE}) 
else()
    message("Using debug")
    target_link_libraries(physx INTERFACE ${PHYSX_LIBS_DEBUG}) 
endif ()