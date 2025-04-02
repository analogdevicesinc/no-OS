no_os_sources_ifdef(CONFIG_ACCEL_ADXL355 ${CMAKE_CURRENT_SOURCE_DIR}/adxl355/adxl355.c)
no_os_sources_ifdef(CONFIG_ACCEL_ADXL355 ${CMAKE_CURRENT_SOURCE_DIR}/adxl355/iio_adxl355.c)
no_os_sources_ifdef(CONFIG_ACCEL_ADXL355 ${CMAKE_CURRENT_SOURCE_DIR}/adxl355/iio_adxl355_trig.c)
target_include_directories(capi_build PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adxl355)