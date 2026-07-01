add_executable(capi_selftest)

target_sources(capi_selftest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/test_framework.c
)

if(CONFIG_CAPI_SELFTEST_BASIC_EXAMPLE)
    target_sources(capi_selftest PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(capi_selftest PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

target_include_directories(capi_selftest PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
)

target_link_libraries(capi_selftest no-os)

# BSP + flash go through the shared cmake/xilinx dispatchers for every platform.
config_platform_sdk(capi_selftest)
add_flash_target(capi_selftest)
post_build_config(capi_selftest)
