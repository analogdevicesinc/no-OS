add_executable(ad738x_fmcz)

# Common sources
target_sources(ad738x_fmcz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_AD738X_FMCZ_BASIC_EXAMPLE)
    target_sources(ad738x_fmcz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example/basic_example.c
    )
    target_include_directories(ad738x_fmcz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example
    )
endif()

# IIO example
if(CONFIG_AD738X_FMCZ_IIO_EXAMPLE)
    target_sources(ad738x_fmcz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(ad738x_fmcz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

target_include_directories(ad738x_fmcz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

# Define platform-specific flags
if(CONFIG_AD738X_FMCZ_USE_STANDARD_SPI)
    target_compile_definitions(ad738x_fmcz PRIVATE USE_STANDARD_SPI)
endif()

target_link_libraries(ad738x_fmcz no-os)

config_platform_sdk(ad738x_fmcz)
add_flash_target(ad738x_fmcz)
post_build_config(ad738x_fmcz)
