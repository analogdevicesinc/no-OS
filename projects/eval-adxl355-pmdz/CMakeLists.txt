add_executable(eval-adxl355-pmdz)

# Common sources
target_sources(eval-adxl355-pmdz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Dummy example
if(CONFIG_ADXL355_PMDZ_DUMMY_EXAMPLE)
    target_sources(eval-adxl355-pmdz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dummy/dummy_example.c
    )
    target_include_directories(eval-adxl355-pmdz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dummy
    )
endif()

# IIO example
if(CONFIG_ADXL355_PMDZ_IIO_EXAMPLE)
    target_sources(eval-adxl355-pmdz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(eval-adxl355-pmdz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

# IIO trigger example
if(CONFIG_ADXL355_PMDZ_IIO_TRIGGER_EXAMPLE)
    target_sources(eval-adxl355-pmdz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_trigger_example/iio_trigger_example.c
    )
    target_include_directories(eval-adxl355-pmdz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_trigger_example
    )
endif()

# IIO LWIP example
if(CONFIG_ADXL355_PMDZ_IIO_LWIP_EXAMPLE)
    target_sources(eval-adxl355-pmdz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_lwip_example/iio_lwip_example.c
    )
    target_include_directories(eval-adxl355-pmdz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_lwip_example
    )
endif()

target_include_directories(eval-adxl355-pmdz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

# Select the target device. The adxl355 driver picks dev_type from one of
# these mutually-exclusive defines (see src/common/common_data.c).
if(CONFIG_ADXL355_PMDZ_DEV_ADXL359)
    target_compile_definitions(eval-adxl355-pmdz PRIVATE -DADXL359_DEV)
elseif(CONFIG_ADXL355_PMDZ_DEV_ADXL357)
    target_compile_definitions(eval-adxl355-pmdz PRIVATE -DADXL357_DEV)
else()
    target_compile_definitions(eval-adxl355-pmdz PRIVATE -DADXL355_DEV)
endif()
target_link_libraries(eval-adxl355-pmdz no-os)

config_platform_sdk(eval-adxl355-pmdz)
add_flash_target(eval-adxl355-pmdz)
post_build_config(eval-adxl355-pmdz)
