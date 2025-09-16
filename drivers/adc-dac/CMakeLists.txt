no_os_sources_ifdef(CONFIG_AD74413R ${CMAKE_CURRENT_SOURCE_DIR}/ad74413r/ad74413r.c)
no_os_sources_ifdef(CONFIG_IIO_AD74413R ${CMAKE_CURRENT_SOURCE_DIR}/ad74413r/iio_ad74413r.c)

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad74413r)