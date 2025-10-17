cmake_minimum_required(VERSION 3.10)
project(HuffmanCompressor CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files
set(SOURCES
    src/main.cpp
    src/Huffman.cpp
)

# Add header directory
include_directories(src)

# Create the executable
add_executable(huffman ${SOURCES})