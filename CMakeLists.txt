cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(D3D12-ABV)

if(MSVC)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /Zc:__cplusplus /std:c++17")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/utfcpp/source)

add_subdirectory(tests)