# MAX14906
no_os_sources_ifdef(CONFIG_DIGITAL_IO_MAX14906
    ${CMAKE_CURRENT_SOURCE_DIR}/max149x6/max149x6-base.c
    ${CMAKE_CURRENT_SOURCE_DIR}/max149x6/max14906.c)
no_os_sources_ifdef(CONFIG_DIGITAL_IO_IIO_MAX14906
    ${CMAKE_CURRENT_SOURCE_DIR}/max149x6/iio_max14906.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/max149x6)

# MAX22915
no_os_sources_ifdef(CONFIG_DIGITAL_IO_MAX22915
    ${CMAKE_CURRENT_SOURCE_DIR}/max22915/max22915.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/max22915)
