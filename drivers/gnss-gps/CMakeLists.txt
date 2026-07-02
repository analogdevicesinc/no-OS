# NMEA UBX
no_os_sources_ifdef(CONFIG_GNSS_GPS_NMEA_UBX
    ${CMAKE_CURRENT_SOURCE_DIR}/nmea_ubx/nmea_ubx.c)
no_os_sources_ifdef(CONFIG_GNSS_GPS_NMEA_UBX
    ${CMAKE_CURRENT_SOURCE_DIR}/../platform/generic/nmea_ubx_gnss.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/nmea_ubx)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../platform/generic)
