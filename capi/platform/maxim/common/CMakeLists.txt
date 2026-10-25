no_os_sources_ifdef(CONFIG_CAPI_ALLOC ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_alloc.c)

if (${TARGET} IN_LIST MAXIM_CAPI_COMMON_TARGETS)
	no_os_sources_ifdef(CONFIG_CAPI_TIME  ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_time.c)
	no_os_sources_ifdef(CONFIG_CAPI_IRQ   ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_irq.c)
	no_os_sources_ifdef(CONFIG_CAPI_GPIO  ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_gpio.c)
	no_os_sources_ifdef(CONFIG_CAPI_DMA   ${CMAKE_CURRENT_SOURCE_DIR}/maxim_capi_dma.c)

	target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
	target_include_directories(no-os PUBLIC
		${MAXIM_LIBRARIES}/PeriphDrivers/Source/SPI
		${MAXIM_LIBRARIES}/PeriphDrivers/Source/DMA)
endif()
