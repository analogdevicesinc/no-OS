add_executable(cn0565)

# Common sources (always compiled)
target_sources(cn0565 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mux_board/mux_board.c
)

target_include_directories(cn0565 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mux_board
)

target_link_libraries(cn0565 no-os m)

config_platform_sdk(cn0565)
add_flash_target(cn0565)
post_build_config(cn0565)
