add_executable(ad5766-sdz)

target_sources(ad5766-sdz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ad5766_sdz.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ad5766_core.c
)

target_include_directories(ad5766-sdz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ad5766-sdz no-os)

config_platform_sdk(ad5766-sdz)
add_flash_target(ad5766-sdz)
post_build_config(ad5766-sdz)
