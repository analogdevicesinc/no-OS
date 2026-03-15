add_executable(iio_demo)

# Common sources
target_sources(iio_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic IIO example
if(CONFIG_IIO_DEMO_IIO_EXAMPLE)
    target_sources(iio_demo PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_compile_definitions(iio_demo PRIVATE IIO_EXAMPLE)
    target_include_directories(iio_demo PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

# IIO timer trigger example
if(CONFIG_IIO_DEMO_IIO_TIMER_TRIGGER_EXAMPLE)
    target_sources(iio_demo PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_timer_trigger_example/iio_timer_trigger_example.c
    )
    target_compile_definitions(iio_demo PRIVATE IIO_TIMER_TRIGGER_EXAMPLE)
    target_include_directories(iio_demo PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_timer_trigger_example
    )
endif()

# IIO software trigger example
if(CONFIG_IIO_DEMO_IIO_SW_TRIGGER_EXAMPLE)
    target_sources(iio_demo PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_sw_trigger_example/iio_sw_trigger_example.c
    )
    target_compile_definitions(iio_demo PRIVATE IIO_SW_TRIGGER_EXAMPLE)
    target_include_directories(iio_demo PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_sw_trigger_example
    )
endif()

target_include_directories(iio_demo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(iio_demo no-os)

config_platform_sdk(iio_demo)
add_flash_target(iio_demo)
post_build_config(iio_demo)
