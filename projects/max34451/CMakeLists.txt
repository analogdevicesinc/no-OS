add_executable(max34451)

# Common sources
target_sources(max34451 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_MAX34451_BASIC_EXAMPLE)
    target_sources(max34451 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(max34451 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# IIO example
if(CONFIG_MAX34451_IIO_EXAMPLE)
    target_sources(max34451 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio/iio_example.c
    )
    target_include_directories(max34451 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio
    )
endif()

target_include_directories(max34451 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(max34451 no-os)

config_platform_sdk(max34451)
add_flash_target(max34451)
post_build_config(max34451)
