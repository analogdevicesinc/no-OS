add_executable(adrv9001)

target_sources(adrv9001 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/headless.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal/no_os_platform.c
)

# demo variant (basic bring-up, no extra flags)
if(CONFIG_ADRV9001_DEMO_EXAMPLE)
    # No additional sources beyond headless.c
endif()

# demo_adrv9002 variant (ADRV9002 RX2TX2 mode)
if(CONFIG_ADRV9001_DEMO_ADRV9002_EXAMPLE)
    target_compile_definitions(adrv9001 PRIVATE ADRV9002_RX2TX2)
endif()

# dma_example variant
if(CONFIG_ADRV9001_DMA_EXAMPLE)
    target_compile_definitions(adrv9001 PRIVATE DMA_EXAMPLE)
endif()

# iio variant (IIO support, standard 2-channel)
if(CONFIG_ADRV9001_IIO_EXAMPLE)
    target_compile_definitions(adrv9001 PRIVATE IIO_SUPPORT)
endif()

# iio_adrv9002 variant (IIO support + ADRV9002 RX2TX2 mode)
if(CONFIG_ADRV9001_IIO_ADRV9002_EXAMPLE)
    target_compile_definitions(adrv9001 PRIVATE IIO_SUPPORT ADRV9002_RX2TX2)
endif()

# Compiler flags required by the Navassa vendor API. These MUST go on the no-os
# target, not adrv9001: the navassa sources compile into the no-os library, and
# ADI_DYNAMIC_PROFILE_LOAD gates the body of adi_adrv9001_profileutil_Parse()
# there (a PRIVATE define on the project target would not reach it, leaving the
# symbol undefined at link).
target_compile_definitions(no-os PUBLIC
    ADI_DYNAMIC_PROFILE_LOAD
    ADI_COMMON_VERBOSE=1
    ADI_ADRV9001_ARM_VERBOSE
    ADI_VALIDATE_PARAMS
)

target_include_directories(adrv9001 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/firmware
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal
)

# The navassa driver (in no-os library) includes the project-supplied
# no_os_platform.h which defines adrv9002_hal_cfg. It must be on the
# no-os include path.
target_include_directories(no-os PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal
)

config_platform_sdk(adrv9001)
add_flash_target(adrv9001)
post_build_config(adrv9001)
target_link_libraries(adrv9001 no-os)
