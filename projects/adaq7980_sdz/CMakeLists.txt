add_executable(adaq7980_sdz)

target_sources(adaq7980_sdz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adaq7980_sdz.c
)

target_include_directories(adaq7980_sdz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(adaq7980_sdz no-os)

config_platform_sdk(adaq7980_sdz)
add_flash_target(adaq7980_sdz)
post_build_config(adaq7980_sdz)
