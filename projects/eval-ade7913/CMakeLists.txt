add_executable(eval-ade7913)

# Common sources
target_sources(eval-ade7913 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interrupt/interrupt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/platform.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
)

# ADE7913 example
if(CONFIG_EVAL_ADE7913_ADE7913_EXAMPLE)
endif()

target_include_directories(eval-ade7913 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interrupt
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(eval-ade7913 no-os)

config_platform_sdk(eval-ade7913)
add_flash_target(eval-ade7913)
post_build_config(eval-ade7913)
