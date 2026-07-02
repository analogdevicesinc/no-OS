add_executable(adt7420-pmdz)

# Common sources
target_sources(adt7420-pmdz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Dummy example
if(CONFIG_ADT7420_PMDZ_DUMMY_EXAMPLE)
    target_sources(adt7420-pmdz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dummy/dummy_example.c
    )
    target_include_directories(adt7420-pmdz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dummy
    )
endif()

# IIO example
if(CONFIG_ADT7420_PMDZ_IIO_EXAMPLE)
    target_sources(adt7420-pmdz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(adt7420-pmdz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

target_include_directories(adt7420-pmdz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(adt7420-pmdz no-os)

config_platform_sdk(adt7420-pmdz)
add_flash_target(adt7420-pmdz)
post_build_config(adt7420-pmdz)
