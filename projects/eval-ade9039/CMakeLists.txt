add_executable(eval-ade9039)

target_sources(eval-ade9039 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/platform.c
)

target_include_directories(eval-ade9039 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(eval-ade9039 no-os)

config_platform_sdk(eval-ade9039)
add_flash_target(eval-ade9039)
post_build_config(eval-ade9039)
