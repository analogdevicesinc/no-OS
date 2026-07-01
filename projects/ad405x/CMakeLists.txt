add_executable(ad405x)

# Common sources (always compiled)
target_sources(ad405x PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example (SPI, AD4050)
if(CONFIG_AD405X_BASIC_EXAMPLE)
    target_sources(ad405x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example/basic_example.c
    )
    target_include_directories(ad405x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example
    )
    target_compile_definitions(ad405x PRIVATE AD405X_SPI AD405X_DEV_TYPE=ID_AD4050)
endif()

# Basic I3C example (I3C, AD4060)
if(CONFIG_AD405X_BASIC_I3C_EXAMPLE)
    target_sources(ad405x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_i3c_example/basic_i3c_example.c
    )
    target_include_directories(ad405x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_i3c_example
    )
    target_compile_definitions(ad405x PRIVATE AD405X_I3C AD405X_INSTANCE_ID=0 AD405X_DEV_TYPE=ID_AD4060)
endif()

# I3C DMA example (I3C + GPDMA, AD4062)
if(CONFIG_AD405X_I3C_DMA_EXAMPLE)
    target_sources(ad405x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/i3c_dma_example/i3c_dma_example.c
    )
    target_include_directories(ad405x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/i3c_dma_example
    )
    target_compile_definitions(ad405x PRIVATE AD405X_I3C AD405X_INSTANCE_ID=0 AD405X_DEV_TYPE=ID_AD4062)
endif()

# IIO example (SPI + IIO, AD4058)
if(CONFIG_AD405X_IIO_EXAMPLE)
    target_sources(ad405x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(ad405x PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
    target_compile_definitions(ad405x PRIVATE AD405X_SPI AD405X_DEV_TYPE=ID_AD4058)
endif()

target_include_directories(ad405x PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

# Board macro derived from the .ioc name, consumed by parameters.h to pick the
# pin configuration (NUCLEO_H563ZI vs NUCLEO_H503RB).
if(BOARD STREQUAL "nucleo-h563zi")
    target_compile_definitions(ad405x PRIVATE NUCLEO_H563ZI)
elseif(BOARD STREQUAL "nucleo-h503rb")
    target_compile_definitions(ad405x PRIVATE NUCLEO_H503RB)
endif()

target_link_libraries(ad405x no-os)

config_platform_sdk(ad405x)
add_flash_target(ad405x)
post_build_config(ad405x)
