no_os_sources_ifdef(CONFIG_ACCEL_ADXL313 ${CMAKE_CURRENT_SOURCE_DIR}/adxl313/adxl313.c)
no_os_sources_ifdef(CONFIG_ACCEL_IIO_ADXL313 ${CMAKE_CURRENT_SOURCE_DIR}/adxl313/iio_adxl313.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adxl313)

no_os_sources_ifdef(CONFIG_ACCEL_ADXL355 ${CMAKE_CURRENT_SOURCE_DIR}/adxl355/adxl355.c)
no_os_sources_ifdef(CONFIG_ACCEL_IIO_ADXL355 ${CMAKE_CURRENT_SOURCE_DIR}/adxl355/iio_adxl355.c)
no_os_sources_ifdef(CONFIG_ACCEL_IIO_ADXL355 ${CMAKE_CURRENT_SOURCE_DIR}/adxl355/iio_adxl355_trig.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adxl355)

no_os_sources_ifdef(CONFIG_ACCEL_ADXL367 ${CMAKE_CURRENT_SOURCE_DIR}/adxl367/adxl367.c)
no_os_sources_ifdef(CONFIG_ACCEL_IIO_ADXL367 ${CMAKE_CURRENT_SOURCE_DIR}/adxl367/iio_adxl367.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adxl367)

no_os_sources_ifdef(CONFIG_ACCEL_ADXL38X ${CMAKE_CURRENT_SOURCE_DIR}/adxl38x/adxl38x.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adxl38x)
