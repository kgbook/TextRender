cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME TextRender)

project(${PROJECT_NAME})

set(project_debug 1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

if (project_debug)
    set(CMAKE_VERBOSE_MAKEFILE ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    add_definitions(-DNDEBUG)
endif ()

include(cpplint)

find_package(PkgConfig)
pkg_check_modules(freetype REQUIRED freetype2)

if (NOT freetype_FOUND)
    message(FATAL_ERROR "freetype2 not found!")
endif()

include_directories(include
        ${freetype_INCLUDE_DIRS})

link_directories(${freetype_LIBRARY_DIRS})

add_subdirectory(source)
add_subdirectory(example)
