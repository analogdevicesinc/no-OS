add_executable(display_demo)

target_sources(display_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/main.c
)

target_include_directories(display_demo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
)

target_link_libraries(display_demo no-os)

config_platform_sdk(display_demo)
add_flash_target(display_demo)
post_build_config(display_demo)
