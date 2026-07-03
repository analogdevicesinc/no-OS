add_executable(iio_demo_freeRTOS)

target_sources(iio_demo_freeRTOS PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

if(CONFIG_IIO_DEMO_FREERTOS_IIO_EXAMPLE)
    target_sources(iio_demo_freeRTOS PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(iio_demo_freeRTOS PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

target_include_directories(iio_demo_freeRTOS PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

# Link freertos_kernel directly (in addition to transitively via no-os) so the
# executable's own sources (main.c) get FreeRTOS.h / FreeRTOSConfig.h on their
# include path. The target is defined later in add_subdirectory(libraries);
# target_link_libraries resolves target names lazily, so the forward reference
# is fine.
target_link_libraries(iio_demo_freeRTOS no-os freertos_kernel)

config_platform_sdk(iio_demo_freeRTOS)
add_flash_target(iio_demo_freeRTOS)
post_build_config(iio_demo_freeRTOS)
