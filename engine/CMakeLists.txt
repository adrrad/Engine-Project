
file(GLOB
        src 
        *.cpp
        rendering/*.cpp 
        rendering/rpstrategies/*.cpp 
        utilities/*.cpp 
        utilities/serialisation/*.cpp 
        utilities/json/*.cpp 
        utilities/bvh/*.cpp 
        components/*.cpp
        core/*.cpp
        gui/*.cpp
        geometry/*.cpp
        acceleration/*.cpp
        assets/*.cpp
        assets/importing/*.cpp
        assets/resources/*.cpp
        physics/*.cpp
        platform/*.cpp
        platform/io/*.cpp
        memory/*.cpp
        gameplay/*.cpp
        gameplay/ecs/*.cpp
        gameplay/ecs/systems/*.cpp
    )

add_library(core ${src})

target_include_directories(core PUBLIC .)
target_link_libraries(core re)