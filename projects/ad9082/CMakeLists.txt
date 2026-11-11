add_executable(ad9082)

target_sources(ad9082 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
)

if(CONFIG_AD9082_BASIC_EXAMPLE)
    target_sources(ad9082 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example/basic_example.c
    )
    target_include_directories(ad9082 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic_example
    )
endif()

if(CONFIG_AD9082_DMA_EXAMPLE)
    target_sources(ad9082 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dma_example/dma_example.c
    )
    target_include_directories(ad9082 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dma_example
    )
endif()

if(CONFIG_AD9082_IIO_EXAMPLE)
    target_sources(ad9082 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example/iio_example.c
    )
    target_include_directories(ad9082 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio_example
    )
endif()

# Hardware profile (JESD mode + datapath). Versal/VPK180 only for now; a single
# profile, selected unconditionally. Add more under profiles/<board>_ad9082/ and
# switch here (e.g. per BOARD) when another Versal board/profile is introduced.
set(AD9082_PROFILE profiles/vpk180_ad9082)

target_include_directories(ad9082 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/${AD9082_PROFILE}
)

# Versal-only project: define PLATFORM_VERSAL for the app sources (parameters.h
# also auto-detects it from the BSP for a Vitis-IDE build).
target_compile_definitions(ad9082 PRIVATE PLATFORM_VERSAL)

# The Vitis 2025 Versal BSP is SDT: driver config tables key on base address and
# the *_Config structs use the SDT layout. Build no-os with -DSDT to match.
set(NOOS_XILINX_SDT ON)

config_platform_sdk(ad9082)
add_flash_target(ad9082)
post_build_config(ad9082)
target_link_libraries(ad9082 no-os)
