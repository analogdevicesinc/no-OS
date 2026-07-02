add_executable(ltc7841)

# Common sources
target_sources(ltc7841 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_LTC7841_BASIC_EXAMPLE)
    target_sources(ltc7841 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(ltc7841 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# IIO example
if(CONFIG_LTC7841_IIO_EXAMPLE)
    target_sources(ltc7841 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(ltc7841 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

target_include_directories(ltc7841 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(ltc7841 no-os)

config_platform_sdk(ltc7841)
add_flash_target(ltc7841)
post_build_config(ltc7841)
