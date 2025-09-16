# ADIS Core Driver (required by all ADIS variants)
no_os_sources_ifdef(CONFIG_IMU_ADIS ${CMAKE_CURRENT_SOURCE_DIR}/adis.c)

# ADIS1646X
no_os_sources_ifdef(CONFIG_IMU_ADIS1646X ${CMAKE_CURRENT_SOURCE_DIR}/adis1646x.c)
no_os_sources_ifdef(CONFIG_IMU_IIO_ADIS1646X ${CMAKE_CURRENT_SOURCE_DIR}/iio_adis1646x.c)

# ADIS1647X
no_os_sources_ifdef(CONFIG_IMU_ADIS1647X ${CMAKE_CURRENT_SOURCE_DIR}/adis1647x.c)
no_os_sources_ifdef(CONFIG_IMU_IIO_ADIS1647X ${CMAKE_CURRENT_SOURCE_DIR}/iio_adis1647x.c)

# ADIS1650X
no_os_sources_ifdef(CONFIG_IMU_ADIS1650X ${CMAKE_CURRENT_SOURCE_DIR}/adis1650x.c)
no_os_sources_ifdef(CONFIG_IMU_IIO_ADIS1650X ${CMAKE_CURRENT_SOURCE_DIR}/iio_adis1650x.c)

# ADIS1654X
no_os_sources_ifdef(CONFIG_IMU_ADIS1654X ${CMAKE_CURRENT_SOURCE_DIR}/adis1654x.c)
no_os_sources_ifdef(CONFIG_IMU_IIO_ADIS1654X ${CMAKE_CURRENT_SOURCE_DIR}/iio_adis1654x.c)

# ADIS1655X
no_os_sources_ifdef(CONFIG_IMU_ADIS1655X ${CMAKE_CURRENT_SOURCE_DIR}/adis1655x.c)
no_os_sources_ifdef(CONFIG_IMU_IIO_ADIS1655X ${CMAKE_CURRENT_SOURCE_DIR}/iio_adis1655x.c)

# ADIS1657X
no_os_sources_ifdef(CONFIG_IMU_ADIS1657X ${CMAKE_CURRENT_SOURCE_DIR}/adis1657x.c)
no_os_sources_ifdef(CONFIG_IMU_IIO_ADIS1657X ${CMAKE_CURRENT_SOURCE_DIR}/iio_adis1657x.c)

# Common IIO functionality for ADIS drivers
no_os_sources_ifdef(CONFIG_IMU_IIO_ADIS1646X ${CMAKE_CURRENT_SOURCE_DIR}/iio_adis.c)
no_os_sources_ifdef(CONFIG_IMU_IIO_ADIS1647X ${CMAKE_CURRENT_SOURCE_DIR}/iio_adis.c)
no_os_sources_ifdef(CONFIG_IMU_IIO_ADIS1650X ${CMAKE_CURRENT_SOURCE_DIR}/iio_adis.c)
no_os_sources_ifdef(CONFIG_IMU_IIO_ADIS1654X ${CMAKE_CURRENT_SOURCE_DIR}/iio_adis.c)
no_os_sources_ifdef(CONFIG_IMU_IIO_ADIS1655X ${CMAKE_CURRENT_SOURCE_DIR}/iio_adis.c)
no_os_sources_ifdef(CONFIG_IMU_IIO_ADIS1657X ${CMAKE_CURRENT_SOURCE_DIR}/iio_adis.c)

no_os_sources_ifdef(CONFIG_IMU_IIO_ADIS1646X ${CMAKE_CURRENT_SOURCE_DIR}/iio_adis_trig.c)
no_os_sources_ifdef(CONFIG_IMU_IIO_ADIS1647X ${CMAKE_CURRENT_SOURCE_DIR}/iio_adis_trig.c)
no_os_sources_ifdef(CONFIG_IMU_IIO_ADIS1650X ${CMAKE_CURRENT_SOURCE_DIR}/iio_adis_trig.c)
no_os_sources_ifdef(CONFIG_IMU_IIO_ADIS1654X ${CMAKE_CURRENT_SOURCE_DIR}/iio_adis_trig.c)
no_os_sources_ifdef(CONFIG_IMU_IIO_ADIS1655X ${CMAKE_CURRENT_SOURCE_DIR}/iio_adis_trig.c)
no_os_sources_ifdef(CONFIG_IMU_IIO_ADIS1657X ${CMAKE_CURRENT_SOURCE_DIR}/iio_adis_trig.c)

# Include directories
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})