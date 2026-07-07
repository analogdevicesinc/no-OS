add_executable(aducm3029_flash_demo)

# Common sources
target_sources(aducm3029_flash_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
)

# Flash example
if(CONFIG_ADUCM_FLASH_DEMO_EXAMPLE)
    target_sources(aducm3029_flash_demo PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/flash_example/flash_example.c
    )
    target_include_directories(aducm3029_flash_demo PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/flash_example
    )
endif()

target_include_directories(aducm3029_flash_demo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(aducm3029_flash_demo no-os)

config_platform_sdk(aducm3029_flash_demo)
add_flash_target(aducm3029_flash_demo)
post_build_config(aducm3029_flash_demo)
