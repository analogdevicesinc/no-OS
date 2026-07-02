add_executable(max31827-evkit)

# Common sources
target_sources(max31827-evkit PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_MAX31827_EVKIT_BASIC_EXAMPLE)
    target_sources(max31827-evkit PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(max31827-evkit PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# IIO example
if(CONFIG_MAX31827_EVKIT_IIO_EXAMPLE)
    target_sources(max31827-evkit PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(max31827-evkit PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

target_include_directories(max31827-evkit PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(max31827-evkit no-os)

config_platform_sdk(max31827-evkit)
add_flash_target(max31827-evkit)
post_build_config(max31827-evkit)
