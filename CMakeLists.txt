cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME TextRender)

project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})
include(cpplint)

find_package(PkgConfig)
pkg_check_modules(freetype REQUIRED freetype2)

if (NOT freetype_FOUND)
    message(FATAL_ERROR "freetype2 not found!")
endif()

include_directories(include
        ${freetype_INCLUDE_DIRS})

link_directories(${freetype_LIBRARY_DIRS})

aux_source_directory(${CMAKE_SOURCE_DIR}/source  src)

add_style_check_target(${PROJECT_NAME}-cpplint "${src}")

add_executable(${PROJECT_NAME} ${src})

target_link_libraries(${PROJECT_NAME} ${freetype_LIBRARIES})
