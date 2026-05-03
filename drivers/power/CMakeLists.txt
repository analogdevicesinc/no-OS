# ades1754
no_os_sources_ifdef(CONFIG_POWER_ADES1754 ${CMAKE_CURRENT_SOURCE_DIR}/ades1754/ades1754.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ades1754)

# adp1050
no_os_sources_ifdef(CONFIG_POWER_ADP1050 ${CMAKE_CURRENT_SOURCE_DIR}/adp1050/adp1050.c)
no_os_sources_ifdef(CONFIG_POWER_IIO_ADP1050 ${CMAKE_CURRENT_SOURCE_DIR}/adp1050/iio_adp1050.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adp1050)

# adp1055
no_os_sources_ifdef(CONFIG_POWER_ADP1055 ${CMAKE_CURRENT_SOURCE_DIR}/adp1055/adp1055.c)
no_os_sources_ifdef(CONFIG_POWER_IIO_ADP1055 ${CMAKE_CURRENT_SOURCE_DIR}/adp1055/iio_adp1055.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adp1055)

# adp5055
no_os_sources_ifdef(CONFIG_POWER_ADP5055 ${CMAKE_CURRENT_SOURCE_DIR}/adp5055/adp5055.c)
no_os_sources_ifdef(CONFIG_POWER_IIO_ADP5055 ${CMAKE_CURRENT_SOURCE_DIR}/adp5055/iio_adp5055.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adp5055)

# lt3074
no_os_sources_ifdef(CONFIG_POWER_LT3074 ${CMAKE_CURRENT_SOURCE_DIR}/lt3074/lt3074.c)
no_os_sources_ifdef(CONFIG_POWER_IIO_LT3074 ${CMAKE_CURRENT_SOURCE_DIR}/lt3074/iio_lt3074.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lt3074)

# lt7170
no_os_sources_ifdef(CONFIG_POWER_LT7170 ${CMAKE_CURRENT_SOURCE_DIR}/lt7170/lt7170.c)
no_os_sources_ifdef(CONFIG_POWER_IIO_LT7170 ${CMAKE_CURRENT_SOURCE_DIR}/lt7170/iio_lt7170.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lt7170)

# lt7182s
no_os_sources_ifdef(CONFIG_POWER_LT7182S ${CMAKE_CURRENT_SOURCE_DIR}/lt7182s/lt7182s.c)
no_os_sources_ifdef(CONFIG_POWER_IIO_LT7182S ${CMAKE_CURRENT_SOURCE_DIR}/lt7182s/iio_lt7182s.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lt7182s)

# lt8491
no_os_sources_ifdef(CONFIG_POWER_LT8491 ${CMAKE_CURRENT_SOURCE_DIR}/lt8491/lt8491.c)
no_os_sources_ifdef(CONFIG_POWER_IIO_LT8491 ${CMAKE_CURRENT_SOURCE_DIR}/lt8491/iio_lt8491.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lt8491)

# lt8722
no_os_sources_ifdef(CONFIG_POWER_LT8722 ${CMAKE_CURRENT_SOURCE_DIR}/lt8722/lt8722.c)
no_os_sources_ifdef(CONFIG_POWER_IIO_LT8722 ${CMAKE_CURRENT_SOURCE_DIR}/lt8722/iio_lt8722.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lt8722)

# ltc2992
no_os_sources_ifdef(CONFIG_POWER_LTC2992 ${CMAKE_CURRENT_SOURCE_DIR}/ltc2992/ltc2992.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ltc2992)

# ltc3337
no_os_sources_ifdef(CONFIG_POWER_LTC3337 ${CMAKE_CURRENT_SOURCE_DIR}/ltc3337/ltc3337.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ltc3337)

# ltc3350
no_os_sources_ifdef(CONFIG_POWER_LTC3350 ${CMAKE_CURRENT_SOURCE_DIR}/ltc3350/ltc3350.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ltc3350)

# ltc4162l
no_os_sources_ifdef(CONFIG_POWER_LTC4162L ${CMAKE_CURRENT_SOURCE_DIR}/ltc4162l/ltc4162l.c)
no_os_sources_ifdef(CONFIG_POWER_IIO_LTC4162L ${CMAKE_CURRENT_SOURCE_DIR}/ltc4162l/iio_ltc4162l.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ltc4162l)

# ltc4296
no_os_sources_ifdef(CONFIG_POWER_LTC4296 ${CMAKE_CURRENT_SOURCE_DIR}/ltc4296/ltc4296.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ltc4296)

# ltc7841
no_os_sources_ifdef(CONFIG_POWER_LTC7841 ${CMAKE_CURRENT_SOURCE_DIR}/ltc7841/ltc7841.c)
no_os_sources_ifdef(CONFIG_POWER_IIO_LTC7841 ${CMAKE_CURRENT_SOURCE_DIR}/ltc7841/iio_ltc7841.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ltc7841)

# ltc7871
no_os_sources_ifdef(CONFIG_POWER_LTC7871 ${CMAKE_CURRENT_SOURCE_DIR}/ltc7871/ltc7871.c)
no_os_sources_ifdef(CONFIG_POWER_IIO_LTC7871 ${CMAKE_CURRENT_SOURCE_DIR}/ltc7871/iio_ltc7871.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ltc7871)

# ltm3360b
no_os_sources_ifdef(CONFIG_POWER_LTM3360B ${CMAKE_CURRENT_SOURCE_DIR}/ltm3360b/ltm3360b.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ltm3360b)

# ltm4686
no_os_sources_ifdef(CONFIG_POWER_LTM4686 ${CMAKE_CURRENT_SOURCE_DIR}/ltm4686/ltm4686.c)
no_os_sources_ifdef(CONFIG_POWER_IIO_LTM4686 ${CMAKE_CURRENT_SOURCE_DIR}/ltm4686/iio_ltm4686.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ltm4686)

# ltp8800
no_os_sources_ifdef(CONFIG_POWER_LTP8800 ${CMAKE_CURRENT_SOURCE_DIR}/ltp8800/ltp8800.c)
no_os_sources_ifdef(CONFIG_POWER_IIO_LTP8800 ${CMAKE_CURRENT_SOURCE_DIR}/ltp8800/iio_ltp8800.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ltp8800)

# max17616
no_os_sources_ifdef(CONFIG_POWER_MAX17616 ${CMAKE_CURRENT_SOURCE_DIR}/max17616/max17616.c)
no_os_sources_ifdef(CONFIG_POWER_IIO_MAX17616 ${CMAKE_CURRENT_SOURCE_DIR}/max17616/iio_max17616.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/max17616)

# max17851
no_os_sources_ifdef(CONFIG_POWER_MAX17851 ${CMAKE_CURRENT_SOURCE_DIR}/max17851/max17851.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/max17851)

# max22216
no_os_sources_ifdef(CONFIG_POWER_MAX22216 ${CMAKE_CURRENT_SOURCE_DIR}/max22216/max22216.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/max22216)

# max42500
no_os_sources_ifdef(CONFIG_POWER_MAX42500 ${CMAKE_CURRENT_SOURCE_DIR}/max42500/max42500.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/max42500)
