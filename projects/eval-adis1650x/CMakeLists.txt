add_executable(eval-adis1650x)

# Common sources
target_sources(eval-adis1650x PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_ADIS1650X_BASIC_EXAMPLE)
    target_sources(eval-adis1650x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example/basic_example.c
    )
    target_include_directories(eval-adis1650x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example
    )
endif()

# IIO trigger example
if(CONFIG_ADIS1650X_IIO_TRIGGER_EXAMPLE)
    target_sources(eval-adis1650x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_trigger_example/iio_trigger_example.c
    )
    target_include_directories(eval-adis1650x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_trigger_example
    )
endif()

target_include_directories(eval-adis1650x PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(eval-adis1650x no-os)

config_platform_sdk(eval-adis1650x)
add_flash_target(eval-adis1650x)
post_build_config(eval-adis1650x)
