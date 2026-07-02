add_executable(eval-ade9153a)

target_sources(eval-ade9153a PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
)

target_include_directories(eval-ade9153a PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
)

target_link_libraries(eval-ade9153a no-os)

config_platform_sdk(eval-ade9153a)
add_flash_target(eval-ade9153a)
post_build_config(eval-ade9153a)
