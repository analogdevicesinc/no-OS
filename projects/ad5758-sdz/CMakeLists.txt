add_executable(ad5758-sdz)

target_sources(ad5758-sdz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/ad5758_sdz.c
)

target_include_directories(ad5758-sdz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
)

target_link_libraries(ad5758-sdz no-os)

config_platform_sdk(ad5758-sdz)
add_flash_target(ad5758-sdz)
post_build_config(ad5758-sdz)
