add_executable(eval-adis16607)

# Common sources
target_sources(eval-adis16607 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# IIO trigger example
if(CONFIG_EVAL_ADIS16607_IIO_TRIGGER_EXAMPLE)
    target_sources(eval-adis16607 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_trigger_example/iio_trigger_example.c
    )
    target_include_directories(eval-adis16607 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_trigger_example
    )
    target_compile_definitions(eval-adis16607 PRIVATE IIO_TRIGGER_EXAMPLE)
endif()

# Basic example
if(CONFIG_EVAL_ADIS16607_BASIC_EXAMPLE)
    target_sources(eval-adis16607 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(eval-adis16607 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

target_include_directories(eval-adis16607 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(eval-adis16607 no-os)

config_platform_sdk(eval-adis16607)
add_flash_target(eval-adis16607)
post_build_config(eval-adis16607)
