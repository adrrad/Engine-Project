
file(GLOB src src/*.cpp)
message(${src})

add_library(core ${src})

target_include_directories(core PUBLIC include)
target_link_libraries(core re)