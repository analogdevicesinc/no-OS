# ADT75
no_os_sources_ifdef(CONFIG_TEMPERATURE_ADT75
    ${CMAKE_CURRENT_SOURCE_DIR}/adt75/adt75.c)
no_os_sources_ifdef(CONFIG_TEMPERATURE_IIO_ADT75
    ${CMAKE_CURRENT_SOURCE_DIR}/adt75/iio_adt75.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adt75)
