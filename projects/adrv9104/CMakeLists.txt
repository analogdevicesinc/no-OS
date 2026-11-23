add_executable(adrv9104)

target_sources(adrv9104 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/headless.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal/no_os_platform.c
)

# demo variant (basic bring-up, no extra flags)
if(CONFIG_ADRV9104_DEMO_EXAMPLE)
    # No additional sources beyond headless.c
endif()

# demo_adrv9002 variant (RX2TX2 mode)
if(CONFIG_ADRV9104_DEMO_ADRV9002_EXAMPLE)
    target_compile_definitions(adrv9104 PRIVATE ADRV9002_RX2TX2)
endif()

# dma_example variant
if(CONFIG_ADRV9104_DMA_EXAMPLE)
    target_compile_definitions(adrv9104 PRIVATE DMA_EXAMPLE)
endif()

# iio variant (IIO support)
if(CONFIG_ADRV9104_IIO_EXAMPLE)
    target_compile_definitions(adrv9104 PRIVATE IIO_SUPPORT)
endif()

# iio_adrv9002 variant (IIO support + RX2TX2 mode)
if(CONFIG_ADRV9104_IIO_ADRV9002_EXAMPLE)
    target_compile_definitions(adrv9104 PRIVATE IIO_SUPPORT ADRV9002_RX2TX2)
endif()

# Compiler flags carried over from the legacy src.mk CFLAGS. These go on the
# no-os target, not adrv9104: the nevis vendor API sources compile into the
# no-os library, so a PRIVATE define on the project target would not reach
# them.
target_compile_definitions(no-os PUBLIC
    ADI_DYNAMIC_PROFILE_LOAD
    ADI_COMMON_VERBOSE=1
    ADI_ADRV9001_ARM_VERBOSE
    ADI_VALIDATE_PARAMS
)

target_include_directories(adrv9104 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/firmware
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal
)

# The nevis driver (in no-os library) includes the project-supplied
# no_os_platform.h. It must be on the no-os include path.
target_include_directories(no-os PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hal
)

config_platform_sdk(adrv9104)
add_flash_target(adrv9104)
post_build_config(adrv9104)
target_link_libraries(adrv9104 no-os)
