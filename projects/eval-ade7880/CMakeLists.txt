add_executable(eval-ade7880)

target_sources(eval-ade7880 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/platform.c
)

target_include_directories(eval-ade7880 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(eval-ade7880 no-os)

config_platform_sdk(eval-ade7880)
add_flash_target(eval-ade7880)
post_build_config(eval-ade7880)
