
file(GLOB 
        src src/rendering/*.cpp 
        src/utilities/*.cpp 
        src/utilities/serialisation/*.cpp 
        src/utilities/json/*.cpp 
        src/utilities/bvh/*.cpp 
        src/components/*.cpp
        src/core/*.cpp
        src/gui/*.cpp
        src/geometry/*.cpp
        src/acceleration/*.cpp
        src/assets/*.cpp
        src/assets/importing/*.cpp
        src/assets/resources/*.cpp
        src/physics/*.cpp
        src/platform/*.cpp
        src/platform/io/*.cpp
        src/editor/*.cpp
        src/editor/gui/*.cpp
    )

add_library(core ${src})

target_include_directories(core PUBLIC include)
target_link_libraries(core re)