add_executable(ad74413r)

# Common sources
target_sources(ad74413r PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Dummy example
if(CONFIG_AD74413R_DUMMY_EXAMPLE)
    target_sources(ad74413r PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dummy/dummy_example.c
    )
    target_include_directories(ad74413r PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dummy
    )
endif()

# IIO example
if(CONFIG_AD74413R_IIO_EXAMPLE)
    target_sources(ad74413r PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(ad74413r PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

# IIO Trigger example
if(CONFIG_AD74413R_IIO_TRIGGER_EXAMPLE)
    target_sources(ad74413r PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_trigger_example/iio_trigger_example.c
    )
    target_include_directories(ad74413r PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_trigger_example
    )
endif()

target_include_directories(ad74413r PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(ad74413r no-os)

config_platform_sdk(ad74413r)
add_flash_target(ad74413r)
post_build_config(ad74413r)
