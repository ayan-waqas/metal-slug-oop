cmake_minimum_required(VERSION 3.16)
project(MetalSlug)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES
        "*.cpp"
)

find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)

add_executable(MetalSlug ${SOURCES})
target_link_libraries(MetalSlug sfml-graphics sfml-window sfml-system sfml-audio)