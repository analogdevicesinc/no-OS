add_executable(ad7124-4sdz)

target_sources(ad7124-4sdz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ad7124-4sdz.c
)

target_include_directories(ad7124-4sdz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ad7124-4sdz no-os)

config_platform_sdk(ad7124-4sdz)
add_flash_target(ad7124-4sdz)
post_build_config(ad7124-4sdz)
