# ADT75
no_os_sources_ifdef(CONFIG_TEMPERATURE_ADT75
    ${CMAKE_CURRENT_SOURCE_DIR}/adt75/adt75.c)
no_os_sources_ifdef(CONFIG_TEMPERATURE_IIO_ADT75
    ${CMAKE_CURRENT_SOURCE_DIR}/adt75/iio_adt75.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adt75)

# LM75
no_os_sources_ifdef(CONFIG_TEMPERATURE_LM75
    ${CMAKE_CURRENT_SOURCE_DIR}/lm75/lm75.c)
no_os_sources_ifdef(CONFIG_TEMPERATURE_IIO_LM75
    ${CMAKE_CURRENT_SOURCE_DIR}/lm75/iio_lm75.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lm75)

# LTC2983
no_os_sources_ifdef(CONFIG_TEMPERATURE_LTC2983
    ${CMAKE_CURRENT_SOURCE_DIR}/ltc2983/ltc2983.c)
no_os_sources_ifdef(CONFIG_TEMPERATURE_IIO_LTC2983
    ${CMAKE_CURRENT_SOURCE_DIR}/ltc2983/iio_ltc2983.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ltc2983)

# MAX31855
no_os_sources_ifdef(CONFIG_TEMPERATURE_MAX31855
    ${CMAKE_CURRENT_SOURCE_DIR}/max31855/max31855.c)
no_os_sources_ifdef(CONFIG_TEMPERATURE_IIO_MAX31855
    ${CMAKE_CURRENT_SOURCE_DIR}/max31855/iio_max31855.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/max31855)
