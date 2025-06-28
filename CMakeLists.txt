cmake_minimum_required(VERSION 3.16)
project(SpoonEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if(WIN32)
    set(CMAKE_SYSTEM_NAME Windows)
elseif(APPLE)
    set(CMAKE_SYSTEM_NAME Darwin)
elseif(UNIX)
    set(CMAKE_SYSTEM_NAME Linux)
endif()

option(SPOON_BUILD_EDITOR "Build the SpoonEngine Editor" OFF)
option(SPOON_BUILD_EXAMPLES "Build example projects" ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Engine/Include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)

add_subdirectory(Engine)

if(SPOON_BUILD_EDITOR)
    add_subdirectory(Editor)
endif()

if(SPOON_BUILD_EXAMPLES)
    add_subdirectory(Examples)
endif()