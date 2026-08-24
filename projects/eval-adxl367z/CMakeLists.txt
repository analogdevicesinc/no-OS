add_executable(eval-adxl367z)

# Common sources (always compiled)
target_sources(eval-adxl367z PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Dummy example
if(CONFIG_EVAL_ADXL367Z_DUMMY_EXAMPLE)
    target_sources(eval-adxl367z PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dummy/dummy_example.c
    )
    target_include_directories(eval-adxl367z PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dummy
    )
endif()

# IIO example
if(CONFIG_EVAL_ADXL367Z_IIO_EXAMPLE)
    target_sources(eval-adxl367z PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(eval-adxl367z PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

target_include_directories(eval-adxl367z PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(eval-adxl367z no-os)

config_platform_sdk(eval-adxl367z)
add_flash_target(eval-adxl367z)
post_build_config(eval-adxl367z)
