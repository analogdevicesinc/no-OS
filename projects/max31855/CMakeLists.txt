add_executable(max31855)

# Common sources
target_sources(max31855 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_MAX31855_BASIC_EXAMPLE)
    target_sources(max31855 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(max31855 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# IIO example
if(CONFIG_MAX31855_IIO_EXAMPLE)
    target_sources(max31855 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio/iio_example.c
    )
    target_include_directories(max31855 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio
    )
endif()

target_include_directories(max31855 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(max31855 no-os)

config_platform_sdk(max31855)
add_flash_target(max31855)
post_build_config(max31855)
