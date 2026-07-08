add_executable(ad4110)

target_sources(ad4110 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/main.c
)

target_include_directories(ad4110 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
)

target_link_libraries(ad4110 no-os)

config_platform_sdk(ad4110)
add_flash_target(ad4110)
post_build_config(ad4110)
