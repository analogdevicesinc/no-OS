add_executable(aducm_blinky_example)

# Common sources
target_sources(aducm_blinky_example PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
)

# Basic (blinky) example
if(CONFIG_ADUCM_BLINKY_BASIC_EXAMPLE)
    target_sources(aducm_blinky_example PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example/basic_example.c
    )
    target_include_directories(aducm_blinky_example PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example
    )
endif()

target_include_directories(aducm_blinky_example PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(aducm_blinky_example no-os)

config_platform_sdk(aducm_blinky_example)
add_flash_target(aducm_blinky_example)
post_build_config(aducm_blinky_example)
