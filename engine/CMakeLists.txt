
file(GLOB 
        src src/renderer/*.cpp 
        src/utilities/*.cpp 
        src/components/*.cpp
        src/core/*.cpp
        src/gui/*.cpp
        src/geometry/*.cpp
        src/acceleration/*.cpp
        src/physics/*.cpp
    )

add_library(core ${src})

target_include_directories(core PUBLIC include)
target_link_libraries(core re)