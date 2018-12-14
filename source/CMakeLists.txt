aux_source_directory(${CMAKE_SOURCE_DIR}/source  lib-src)

add_library(${PROJECT_NAME} STATIC ${lib-src})

add_style_check_target(lib-cpplint "${lib-src}")