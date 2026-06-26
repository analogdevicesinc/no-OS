# adxrs290
no_os_sources_ifdef(CONFIG_GYRO_ADXRS290 ${CMAKE_CURRENT_SOURCE_DIR}/adxrs290/adxrs290.c)
no_os_sources_ifdef(CONFIG_GYRO_IIO_ADXRS290 ${CMAKE_CURRENT_SOURCE_DIR}/adxrs290/iio_adxrs290.c)
no_os_sources_ifdef(CONFIG_GYRO_IIO_ADXRS290 ${CMAKE_CURRENT_SOURCE_DIR}/adxrs290/iio_adxrs290_trig.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adxrs290)

# adxrs453
no_os_sources_ifdef(CONFIG_GYRO_ADXRS453 ${CMAKE_CURRENT_SOURCE_DIR}/adxrs453/adxrs453.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adxrs453)
