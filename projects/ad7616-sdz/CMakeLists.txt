add_executable(ad7616-sdz)

target_sources(ad7616-sdz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ad7616_sdz.c
)

target_include_directories(ad7616-sdz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ad7616-sdz no-os)

config_platform_sdk(ad7616-sdz)
add_flash_target(ad7616-sdz)
post_build_config(ad7616-sdz)
