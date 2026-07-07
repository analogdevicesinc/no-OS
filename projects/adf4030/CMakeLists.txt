add_executable(adf4030)

# Common sources
target_sources(adf4030 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_ADF4030_BASIC_EXAMPLE)
    target_sources(adf4030 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(adf4030 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# IIO example
if(CONFIG_ADF4030_IIO_EXAMPLE)
    target_sources(adf4030 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio/iio_example.c
    )
    target_include_directories(adf4030 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio
    )
endif()

target_include_directories(adf4030 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(adf4030 no-os)

config_platform_sdk(adf4030)
add_flash_target(adf4030)
post_build_config(adf4030)
