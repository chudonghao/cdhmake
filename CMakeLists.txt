cmake_minimum_required(VERSION 3.8)
project(cdhmake)

set(CMAKE_CXX_STANDARD 11)

add_library(cdhmake_base SHARED src/cdhmake.cpp src/cdhmake.h)

set(SOURCE_FILES src/main.cpp src/build_system.cpp src/build_system.h)
add_executable(cdhmake ${SOURCE_FILES})
target_link_libraries(cdhmake
        cdhmake_base
        boost_filesystem
        boost_system
        boost_program_options
        boost_serialization
        dl
        )
