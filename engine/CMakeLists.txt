
file(GLOB 
        src src/rendering/*.cpp 
        src/utilities/*.cpp 
        src/utilities/serialisation/*.cpp 
        src/components/*.cpp
        src/core/*.cpp
        src/gui/*.cpp
        src/geometry/*.cpp
        src/acceleration/*.cpp
        src/physics/*.cpp
        src/platform/*.cpp
        src/editor/*.cpp
    )

add_library(core ${src})

target_include_directories(core PUBLIC include)
target_link_libraries(core re)