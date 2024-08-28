cmake_minimum_required(VERSION 3.21.0)
project("6502_emulator")
SET(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib 3.0 REQUIRED)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")

set(SFEM_SOURCE_DIR
    src
)
set(SFEM_SOURCE
    ${SFEM_SOURCE_DIR}/HotReload/filewatcher.cpp
    ${SFEM_SOURCE_DIR}/sfem.cpp
    ${SFEM_SOURCE_DIR}/processor.cpp
)
add_executable(sfem ${SFEM_SOURCE})
target_link_libraries(sfem raylib)

set(HEADERS_DIR
    include
)
target_include_directories(sfem PUBLIC ${HEADERS_DIR})
