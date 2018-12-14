aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}  example-src)

add_executable(example ${example-src})

add_style_check_target(example-cpplint "${example-src}")

target_link_libraries(example ${PROJECT_NAME} ${freetype_LIBRARIES})