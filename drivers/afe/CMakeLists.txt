# AD4110
no_os_sources_ifdef(CONFIG_AFE_AD4110 ${CMAKE_CURRENT_SOURCE_DIR}/ad4110/ad4110.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad4110)

# AD413X
no_os_sources_ifdef(CONFIG_AFE_AD413X ${CMAKE_CURRENT_SOURCE_DIR}/ad413x/ad413x.c)
no_os_sources_ifdef(CONFIG_AFE_IIO_AD413X ${CMAKE_CURRENT_SOURCE_DIR}/ad413x/iio_ad413x.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad413x)

# AD5940
no_os_sources_ifdef(CONFIG_AFE_AD5940 ${CMAKE_CURRENT_SOURCE_DIR}/ad5940/ad5940.c)
no_os_sources_ifdef(CONFIG_AFE_IIO_AD5940 ${CMAKE_CURRENT_SOURCE_DIR}/ad5940/iio_ad5940.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5940)