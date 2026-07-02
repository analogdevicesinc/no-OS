add_executable(adalm-mmsc)

# Common sources
target_sources(adalm-mmsc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# IIO example
if(CONFIG_ADALM_MMSC_IIO_EXAMPLE)
    target_sources(adalm-mmsc PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio/iio_example.c
    )
    target_include_directories(adalm-mmsc PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio
    )
endif()

target_include_directories(adalm-mmsc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(adalm-mmsc no-os)

config_platform_sdk(adalm-mmsc)
add_flash_target(adalm-mmsc)
post_build_config(adalm-mmsc)
