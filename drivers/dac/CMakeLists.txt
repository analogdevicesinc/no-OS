no_os_sources_ifdef(CONFIG_DAC_DEMO ${CMAKE_CURRENT_SOURCE_DIR}/dac_demo/dac_demo.c)
no_os_sources_ifdef(CONFIG_DAC_DEMO ${CMAKE_CURRENT_SOURCE_DIR}/dac_demo/iio_dac_demo.c)

target_include_directories(capi_build PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/dac_demo)