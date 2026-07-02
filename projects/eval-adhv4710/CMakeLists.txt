add_executable(eval-adhv4710)

target_sources(eval-adhv4710 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/platform.c
)

if(CONFIG_EVAL_ADHV4710_ADHV4710_EXAMPLE)
endif()

target_include_directories(eval-adhv4710 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(eval-adhv4710 no-os)

config_platform_sdk(eval-adhv4710)
add_flash_target(eval-adhv4710)
post_build_config(eval-adhv4710)
