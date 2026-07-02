add_executable(eval-ade7953)

# Common sources
target_sources(eval-ade7953 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interrupt/interrupt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/platform.c
)

# Basic example
if(CONFIG_EVAL_ADE7953_BASIC_EXAMPLE)
endif()

target_include_directories(eval-ade7953 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interrupt
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(eval-ade7953 no-os)

config_platform_sdk(eval-ade7953)
add_flash_target(eval-ade7953)
post_build_config(eval-ade7953)
