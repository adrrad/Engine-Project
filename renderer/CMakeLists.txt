
file(GLOB src src/*.cpp src/components/*.cpp)

add_library(core ${src})

target_include_directories(core PUBLIC include)
target_link_libraries(core re)