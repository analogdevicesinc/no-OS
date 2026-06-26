# ad7156
no_os_sources_ifdef(CONFIG_CDC_AD7156 ${CMAKE_CURRENT_SOURCE_DIR}/ad7156/ad7156.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad7156)

# ad7746
no_os_sources_ifdef(CONFIG_CDC_AD7746 ${CMAKE_CURRENT_SOURCE_DIR}/ad7746/ad7746.c)
no_os_sources_ifdef(CONFIG_CDC_IIO_AD7746 ${CMAKE_CURRENT_SOURCE_DIR}/ad7746/iio_ad7746.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad7746)
