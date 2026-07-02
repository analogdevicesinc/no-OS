add_executable(ad463x_fmcz)

# Common sources
target_sources(ad463x_fmcz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
)

# Basic example
if(CONFIG_AD463X_FMCZ_BASIC_EXAMPLE)
    target_sources(ad463x_fmcz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(ad463x_fmcz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# IIO example
if(CONFIG_AD463X_FMCZ_IIO_EXAMPLE)
    target_sources(ad463x_fmcz PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio/iio_example.c
    )
    target_include_directories(ad463x_fmcz PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio
    )
endif()

target_include_directories(ad463x_fmcz PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

# STM32 uses standard SPI; the SPI-engine offload path (clkgen + spi_engine
# offload) is compiled in but disabled at runtime (offload_enable=false).
if(CONFIG_AD463X_FMCZ_USE_STANDARD_SPI)
    target_compile_definitions(ad463x_fmcz PRIVATE USE_STANDARD_SPI)
endif()

# Device sub-variant define consumed by common_data.c / the examples. The
# default (AD4630-24) needs no macro: the sources use #if, so an undefined
# symbol evaluates to 0 and falls through to the AD4630-24 branch.
if(CONFIG_AD463X_FMCZ_DEV_ADAQ4224)
    target_compile_definitions(ad463x_fmcz PRIVATE ADAQ4224_DEV)
elseif(CONFIG_AD463X_FMCZ_DEV_ADAQ4216)
    target_compile_definitions(ad463x_fmcz PRIVATE ADAQ4216_DEV)
elseif(CONFIG_AD463X_FMCZ_DEV_AD4030)
    target_compile_definitions(ad463x_fmcz PRIVATE AD4030_DEV)
elseif(CONFIG_AD463X_FMCZ_DEV_AD4630_20)
    target_compile_definitions(ad463x_fmcz PRIVATE AD4630_20_DEV)
endif()

target_link_libraries(ad463x_fmcz no-os m)

config_platform_sdk(ad463x_fmcz)
add_flash_target(ad463x_fmcz)
post_build_config(ad463x_fmcz)
