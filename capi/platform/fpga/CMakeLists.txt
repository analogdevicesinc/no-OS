# Fabric-neutral CAPI backends for ADI AXI soft-IP.
#
# Unlike maxim/stm32/xilinx, "fpga" is not a build PLATFORM — the active platform
# is still the PS vendor (e.g. xilinx). Each backend owns its core's register map
# and accesses it through CAPI_PLATFORM_IO_*, following the matching drivers/axi_core driver
# but calling only CAPI services. capi/CMakeLists.txt adds this directory on top
# of the active platform/${PLATFORM} when CONFIG_CAPI_FPGA is set.

# DMA — ADI AXI DMAC. Self-contained: the backend owns the DMAC register map and
# accesses it through CAPI_PLATFORM_IO_*, so it needs no no-OS axi_core driver. Gated only on
# the neutral CONFIG_CAPI_DMA.
no_os_sources_ifdef(CONFIG_CAPI_DMA ${CMAKE_CURRENT_SOURCE_DIR}/fpga_capi_dma.c)

# SPI — ADI SPI Engine. Self-contained: the backend owns the SPI Engine register
# and command map and accesses it through CAPI_PLATFORM_IO_*; the clock, trigger and
# offload DMA are peer CAPI handles. Gated only on the neutral CONFIG_CAPI_SPI.
no_os_sources_ifdef(CONFIG_CAPI_SPI ${CMAKE_CURRENT_SOURCE_DIR}/fpga_capi_spi.c)

# CLK — ADI AXI CLKGEN. Self-contained: the backend owns the CLKGEN register map
# and accesses it through CAPI_PLATFORM_IO_*. The rate path programs a Xilinx MMCM over DRP,
# so it is valid only on Xilinx fabric. Gated only on CONFIG_CAPI_CLK.
no_os_sources_ifdef(CONFIG_CAPI_CLK ${CMAKE_CURRENT_SOURCE_DIR}/fpga_capi_clk.c)

# TIMER (PWM mode) — ADI AXI PWM generator (offload trigger). Self-contained:
# the backend owns the PWM generator register map and accesses it through CAPI_PLATFORM_IO_*.
# Gated only on CONFIG_CAPI_TIMER.
no_os_sources_ifdef(CONFIG_CAPI_TIMER ${CMAKE_CURRENT_SOURCE_DIR}/fpga_capi_timer_pwmgen.c)

# Register access uses the capi_platform.h CAPI_PLATFORM_IO_* accessors;
# the active platform binds them in <platform>_capi_platform.h. Without one the
# capi_platform.h defaults read 0 and drop writes, so refuse to configure.
set(FPGA_CAPI_IO_HEADER ${PLATFORM}_capi_platform.h)
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../${PLATFORM}/${FPGA_CAPI_IO_HEADER})
	message(FATAL_ERROR "capi/platform/fpga needs capi/platform/${PLATFORM}/"
		"${FPGA_CAPI_IO_HEADER} to bind the CAPI_PLATFORM_IO_* accessors")
endif()
# Force-include capi_platform.h, then the platform header that overrides its
# defaults, so the sources' own #include "capi_platform.h" is a no-op.
set_source_files_properties(
	${CMAKE_CURRENT_SOURCE_DIR}/fpga_capi_dma.c
	${CMAKE_CURRENT_SOURCE_DIR}/fpga_capi_spi.c
	${CMAKE_CURRENT_SOURCE_DIR}/fpga_capi_clk.c
	${CMAKE_CURRENT_SOURCE_DIR}/fpga_capi_timer_pwmgen.c
	TARGET_DIRECTORY no-os
	PROPERTIES COMPILE_OPTIONS "-include;capi_platform.h;-include;${FPGA_CAPI_IO_HEADER}")

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
