add_executable(adxrs290-pmdz)

# Common sources
target_sources(adxrs290-pmdz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# IIO example
if(CONFIG_ADXRS290_PMDZ_IIO_EXAMPLE)
    target_sources(adxrs290-pmdz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(adxrs290-pmdz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

# IIO trigger example
if(CONFIG_ADXRS290_PMDZ_IIO_TRIGGER_EXAMPLE)
    target_sources(adxrs290-pmdz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_trigger_example/iio_trigger_example.c
    )
    target_include_directories(adxrs290-pmdz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_trigger_example
    )
endif()

# IIO timer trigger example
if(CONFIG_ADXRS290_PMDZ_IIO_TIMER_TRIGGER_EXAMPLE)
    target_sources(adxrs290-pmdz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_timer_trigger_example/iio_timer_trigger_example.c
    )
    target_include_directories(adxrs290-pmdz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_timer_trigger_example
    )
endif()

target_include_directories(adxrs290-pmdz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(adxrs290-pmdz no-os)

config_platform_sdk(adxrs290-pmdz)
add_flash_target(adxrs290-pmdz)
post_build_config(adxrs290-pmdz)
