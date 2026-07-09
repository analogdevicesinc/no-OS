add_executable(ad9361)

# XILINX_ARCH is set by drivers/platform/xilinx/toolchain.cmake (value like
# "ps7_cortexa9_0", "psu_cortexa53_0", "sys_mb_0") before CMakeLists runs.
# main.c uses #ifdef PLATFORM_MB to select PL (AXI) SPI/GPIO on MicroBlaze
# vs PS (PSS) SPI/GPIO on Zynq-7000/ZynqMP.  Emit PLATFORM_MB for kcu105.
if(XILINX_ARCH MATCHES "sys_mb")
    target_compile_definitions(ad9361 PRIVATE PLATFORM_MB)
endif()

target_sources(ad9361 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
)

# XILINX_PLATFORM is always needed so main.c takes the Xilinx code path.
target_compile_definitions(ad9361 PRIVATE XILINX_PLATFORM)

# demo variant — standard bring-up, no extra flags
if(CONFIG_AD9361_DEMO_EXAMPLE)
    # No additional sources or defines needed
endif()

# iio variant — IIO/IIOD support over UART (IIOD=y in legacy Make)
if(CONFIG_AD9361_IIO_EXAMPLE)
    target_compile_definitions(ad9361 PRIVATE IIO_SUPPORT)
endif()

# dma-example variant — DMA-based data capture/playback
if(CONFIG_AD9361_DMA_EXAMPLE)
    target_compile_definitions(ad9361 PRIVATE DMA_EXAMPLE)
endif()

# dma-irq-example variant — DMA example with IRQ-driven completion
if(CONFIG_AD9361_DMA_IRQ_EXAMPLE)
    target_compile_definitions(ad9361 PRIVATE DMA_EXAMPLE DMA_IRQ_ENABLE)
endif()

# fmcomms5 demo variant — dual-transceiver FMComms5 board
if(CONFIG_AD9361_DEMO_FMCOMMS5_EXAMPLE)
    target_compile_definitions(ad9361 PRIVATE FMCOMMS5)
endif()

# fmcomms5 iio variant — dual-transceiver FMComms5 board with IIO/IIOD
if(CONFIG_AD9361_IIO_FMCOMMS5_EXAMPLE)
    target_compile_definitions(ad9361 PRIVATE FMCOMMS5 IIO_SUPPORT)
endif()

target_include_directories(ad9361 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# app_config.h and parameters.h are in src/; put them on the no-os include
# path so driver sources that #include them (through ad9361_api.h chain) resolve.
target_include_directories(no-os PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

config_platform_sdk(ad9361)
add_flash_target(ad9361)
post_build_config(ad9361)
target_link_libraries(ad9361 no-os)
