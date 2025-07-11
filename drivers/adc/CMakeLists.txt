no_os_sources_ifdef(CONFIG_ADC_DEMO ${CMAKE_CURRENT_SOURCE_DIR}/adc_demo/adc_demo.c)
no_os_sources_ifdef(CONFIG_ADC_DEMO ${CMAKE_CURRENT_SOURCE_DIR}/adc_demo/iio_adc_demo.c)

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adc_demo)