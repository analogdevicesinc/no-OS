add_executable(ad7616-st)

# Common sources
target_sources(ad7616-st PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# IIO example
if(CONFIG_AD7616_ST_IIO_EXAMPLE)
    target_sources(ad7616-st PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio/iio_example.c
    )
    target_include_directories(ad7616-st PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio
    )
endif()

target_include_directories(ad7616-st PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(ad7616-st no-os m)

config_platform_sdk(ad7616-st)
add_flash_target(ad7616-st)
post_build_config(ad7616-st)
