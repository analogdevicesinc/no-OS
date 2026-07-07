add_executable(iio_aducm3029)

# Single main.c drives both the serial_print and IIO variants; the IIO code
# path is compiled in when CONFIG_IIO is enabled.
target_sources(iio_aducm3029 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
)

target_include_directories(iio_aducm3029 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(CONFIG_IIO)
    target_compile_definitions(iio_aducm3029 PRIVATE IIO_SUPPORT)
endif()

target_link_libraries(iio_aducm3029 no-os)

config_platform_sdk(iio_aducm3029)
add_flash_target(iio_aducm3029)
post_build_config(iio_aducm3029)
