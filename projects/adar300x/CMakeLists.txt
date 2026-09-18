add_executable(adar300x)

target_sources(adar300x PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

if(CONFIG_ADAR300X_BASIC_EXAMPLE)
    target_sources(adar300x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example/basic_example.c
    )
    target_include_directories(adar300x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example
    )
endif()

target_include_directories(adar300x PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
)

# Vitis 2025+ emits SDT BSPs whose driver LookupConfig() matches on base
# address rather than the classic DeviceId. The BSP is compiled with -DSDT, so
# the app must match or every SPI/GPIO lookup resolves against a device id the
# SDT config tables do not contain and init fails.
set(NOOS_XILINX_SDT ON)

config_platform_sdk(adar300x)
add_flash_target(adar300x)
post_build_config(adar300x)
target_link_libraries(adar300x no-os)
