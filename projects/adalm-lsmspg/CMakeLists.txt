add_executable(adalm-lsmspg)

# Common sources
target_sources(adalm-lsmspg PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# IIO example
if(CONFIG_ADALM_LSMSPG_IIO_EXAMPLE)
    target_sources(adalm-lsmspg PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(adalm-lsmspg PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

# Curvetrace example
if(CONFIG_ADALM_LSMSPG_CURVETRACE_EXAMPLE)
    target_sources(adalm-lsmspg PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/curvetrace_example/curvetrace_example.c
    )
    target_include_directories(adalm-lsmspg PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/curvetrace_example
    )
endif()

target_include_directories(adalm-lsmspg PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/src/examples
)

target_link_libraries(adalm-lsmspg no-os)

config_platform_sdk(adalm-lsmspg)
add_flash_target(adalm-lsmspg)
post_build_config(adalm-lsmspg)
