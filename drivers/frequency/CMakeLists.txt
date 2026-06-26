# ad9508
no_os_sources_ifdef(CONFIG_FREQUENCY_AD9508 ${CMAKE_CURRENT_SOURCE_DIR}/ad9508/ad9508.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad9508)

# ad9517
no_os_sources_ifdef(CONFIG_FREQUENCY_AD9517 ${CMAKE_CURRENT_SOURCE_DIR}/ad9517/ad9517.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad9517)

# ad9523
no_os_sources_ifdef(CONFIG_FREQUENCY_AD9523 ${CMAKE_CURRENT_SOURCE_DIR}/ad9523/ad9523.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad9523)

# ad9528
no_os_sources_ifdef(CONFIG_FREQUENCY_AD9528 ${CMAKE_CURRENT_SOURCE_DIR}/ad9528/ad9528.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad9528)

# ad9545 (SPI and I2C variants)
no_os_sources_ifdef(CONFIG_FREQUENCY_AD9545
    ${CMAKE_CURRENT_SOURCE_DIR}/ad9545/ad9545.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ad9545/ad9545_i2c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ad9545/ad9545_spi.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad9545)

# ad9553
no_os_sources_ifdef(CONFIG_FREQUENCY_AD9553 ${CMAKE_CURRENT_SOURCE_DIR}/ad9553/ad9553.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad9553)

# ad9833
no_os_sources_ifdef(CONFIG_FREQUENCY_AD9833 ${CMAKE_CURRENT_SOURCE_DIR}/ad9833/ad9833.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad9833)

# adf4030
no_os_sources_ifdef(CONFIG_FREQUENCY_ADF4030 ${CMAKE_CURRENT_SOURCE_DIR}/adf4030/adf4030.c)
no_os_sources_ifdef(CONFIG_FREQUENCY_IIO_ADF4030 ${CMAKE_CURRENT_SOURCE_DIR}/adf4030/iio_adf4030.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adf4030)

# adf4106
no_os_sources_ifdef(CONFIG_FREQUENCY_ADF4106 ${CMAKE_CURRENT_SOURCE_DIR}/adf4106/adf4106.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adf4106)

# adf4153
no_os_sources_ifdef(CONFIG_FREQUENCY_ADF4153 ${CMAKE_CURRENT_SOURCE_DIR}/adf4153/adf4153.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adf4153)

# adf4156
no_os_sources_ifdef(CONFIG_FREQUENCY_ADF4156 ${CMAKE_CURRENT_SOURCE_DIR}/adf4156/adf4156.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adf4156)

# adf4157
no_os_sources_ifdef(CONFIG_FREQUENCY_ADF4157 ${CMAKE_CURRENT_SOURCE_DIR}/adf4157/adf4157.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adf4157)

# adf4350
no_os_sources_ifdef(CONFIG_FREQUENCY_ADF4350 ${CMAKE_CURRENT_SOURCE_DIR}/adf4350/adf4350.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adf4350)

# adf4368
no_os_sources_ifdef(CONFIG_FREQUENCY_ADF4368 ${CMAKE_CURRENT_SOURCE_DIR}/adf4368/adf4368.c)
no_os_sources_ifdef(CONFIG_FREQUENCY_IIO_ADF4368 ${CMAKE_CURRENT_SOURCE_DIR}/adf4368/iio_adf4368.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adf4368)

# adf4371
no_os_sources_ifdef(CONFIG_FREQUENCY_ADF4371 ${CMAKE_CURRENT_SOURCE_DIR}/adf4371/adf4371.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adf4371)

# adf4377
no_os_sources_ifdef(CONFIG_FREQUENCY_ADF4377 ${CMAKE_CURRENT_SOURCE_DIR}/adf4377/adf4377.c)
no_os_sources_ifdef(CONFIG_FREQUENCY_IIO_ADF4377 ${CMAKE_CURRENT_SOURCE_DIR}/adf4377/iio_adf4377.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adf4377)

# adf4382
no_os_sources_ifdef(CONFIG_FREQUENCY_ADF4382 ${CMAKE_CURRENT_SOURCE_DIR}/adf4382/adf4382.c)
no_os_sources_ifdef(CONFIG_FREQUENCY_IIO_ADF4382 ${CMAKE_CURRENT_SOURCE_DIR}/adf4382/iio_adf4382.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adf4382)

# adf5355
no_os_sources_ifdef(CONFIG_FREQUENCY_ADF5355 ${CMAKE_CURRENT_SOURCE_DIR}/adf5355/adf5355.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adf5355)

# adf5611
no_os_sources_ifdef(CONFIG_FREQUENCY_ADF5611 ${CMAKE_CURRENT_SOURCE_DIR}/adf5611/adf5611.c)
no_os_sources_ifdef(CONFIG_FREQUENCY_IIO_ADF5611 ${CMAKE_CURRENT_SOURCE_DIR}/adf5611/iio_adf5611.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adf5611)

# adf5902
no_os_sources_ifdef(CONFIG_FREQUENCY_ADF5902 ${CMAKE_CURRENT_SOURCE_DIR}/adf5902/adf5902.c)
no_os_sources_ifdef(CONFIG_FREQUENCY_IIO_ADF5902 ${CMAKE_CURRENT_SOURCE_DIR}/adf5902/iio_adf5902.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adf5902)

# admfm2000
no_os_sources_ifdef(CONFIG_FREQUENCY_ADMFM2000 ${CMAKE_CURRENT_SOURCE_DIR}/admfm2000/admfm2000.c)
no_os_sources_ifdef(CONFIG_FREQUENCY_IIO_ADMFM2000 ${CMAKE_CURRENT_SOURCE_DIR}/admfm2000/iio_admfm2000.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/admfm2000)

# admv1013
no_os_sources_ifdef(CONFIG_FREQUENCY_ADMV1013 ${CMAKE_CURRENT_SOURCE_DIR}/admv1013/admv1013.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/admv1013)

# admv1014
no_os_sources_ifdef(CONFIG_FREQUENCY_ADMV1014 ${CMAKE_CURRENT_SOURCE_DIR}/admv1014/admv1014.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/admv1014)

# adrf6780
no_os_sources_ifdef(CONFIG_FREQUENCY_ADRF6780 ${CMAKE_CURRENT_SOURCE_DIR}/adrf6780/adrf6780.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adrf6780)

# hmc7044
no_os_sources_ifdef(CONFIG_FREQUENCY_HMC7044 ${CMAKE_CURRENT_SOURCE_DIR}/hmc7044/hmc7044.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/hmc7044)

# ltc6953
no_os_sources_ifdef(CONFIG_FREQUENCY_LTC6953 ${CMAKE_CURRENT_SOURCE_DIR}/ltc6953/ltc6953.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ltc6953)
