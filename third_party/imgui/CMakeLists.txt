# ---------------------------------------------------------------------------------
# Glad lib
# ---------------------------------------------------------------------------------
include_directories(include)
include_directories(#${TARGET_NAME} PRIVATE
        include
        ${CMAKE_SOURCE_DIR}/third_party/glad/include
        ${CMAKE_SOURCE_DIR}/third_party/glfw//include
		)
file(GLOB target_src "src/*.cpp" )
add_library(imgui STATIC ${target_src})