# MAXM86161
no_os_sources_ifdef(CONFIG_HEALTH_MAXM86161
    ${CMAKE_CURRENT_SOURCE_DIR}/maxm86161/maxm86161.c)
no_os_sources_ifdef(CONFIG_HEALTH_IIO_MAXM86161
    ${CMAKE_CURRENT_SOURCE_DIR}/maxm86161/iio_maxm86161.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/maxm86161)
