# CAPI (Common API) abstraction layer.
#
# The dispatcher sources in src/ are platform-neutral and gated only on their
# CONFIG_CAPI_<MODULE> symbol. The platform-specific backends live in
# platform/<platform>/ and are pulled in for the active PLATFORM only; the
# PS-vs-PL / Zynq-vs-ZynqMP selection happens inside those C sources via #ifdef
# on the BSP-provided XPAR_* macros (the single guard we rely on).

no_os_sources_ifdef(CONFIG_CAPI_UART  ${CMAKE_CURRENT_SOURCE_DIR}/src/capi_uart.c)
no_os_sources_ifdef(CONFIG_CAPI_ALLOC ${CMAKE_CURRENT_SOURCE_DIR}/src/capi_alloc.c)
no_os_sources_ifdef(CONFIG_CAPI_TIME  ${CMAKE_CURRENT_SOURCE_DIR}/src/capi_time.c)
no_os_sources_ifdef(CONFIG_CAPI_GPIO  ${CMAKE_CURRENT_SOURCE_DIR}/src/capi_gpio.c)
no_os_sources_ifdef(CONFIG_CAPI_IRQ   ${CMAKE_CURRENT_SOURCE_DIR}/src/capi_irq.c)
no_os_sources_ifdef(CONFIG_CAPI_SPI   ${CMAKE_CURRENT_SOURCE_DIR}/src/capi_spi.c)
no_os_sources_ifdef(CONFIG_CAPI_TIMER ${CMAKE_CURRENT_SOURCE_DIR}/src/capi_timer.c)
no_os_sources_ifdef(CONFIG_CAPI_I2C   ${CMAKE_CURRENT_SOURCE_DIR}/src/capi_i2c.c)
no_os_sources_ifdef(CONFIG_CAPI_DMA   ${CMAKE_CURRENT_SOURCE_DIR}/src/capi_dma.c)

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/platform/${PLATFORM}/CMakeLists.txt)
	add_subdirectory(platform/${PLATFORM})
endif()
