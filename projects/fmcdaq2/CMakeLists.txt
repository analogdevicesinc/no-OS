add_executable(fmcdaq2)

# XILINX_ARCH is set by drivers/platform/xilinx/toolchain.cmake (value like
# "ps7_cortexa9_0", "psu_cortexa53_0", "sys_mb_0") before CMakeLists runs.
# fmcdaq2.c uses #ifdef PLATFORM_MB to select PL (AXI) SPI/GPIO on MicroBlaze
# vs PS (PSS) SPI/GPIO on Zynq-7000/ZynqMP.  Emit PLATFORM_MB for kcu105.
if(XILINX_ARCH MATCHES "sys_mb")
    target_compile_definitions(fmcdaq2 PRIVATE PLATFORM_MB)
endif()

target_sources(fmcdaq2 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/fmcdaq2.c
)

# Bare #ifdef selectors in fmcdaq2.c — emit compile definitions per variant.
# XILINX_PLATFORM is needed so fmcdaq2.c takes the Xilinx code path
# (not ALTERA_PLATFORM) and calls Xil_DCacheInvalidateRange etc.
target_compile_definitions(fmcdaq2 PRIVATE XILINX_PLATFORM)

if(CONFIG_FMCDAQ2_DMA_EXAMPLE)
    target_compile_definitions(fmcdaq2 PRIVATE DMA_EXAMPLE)
endif()

if(CONFIG_FMCDAQ2_IIO_EXAMPLE)
    target_compile_definitions(fmcdaq2 PRIVATE IIO_SUPPORT)
endif()

target_include_directories(fmcdaq2 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
)

config_platform_sdk(fmcdaq2)
add_flash_target(fmcdaq2)
post_build_config(fmcdaq2)
target_link_libraries(fmcdaq2 no-os)
