# ada4250
no_os_sources_ifdef(CONFIG_AMPLIFIERS_ADA4250 ${CMAKE_CURRENT_SOURCE_DIR}/ada4250/ada4250.c)
no_os_sources_ifdef(CONFIG_AMPLIFIERS_IIO_ADA4250 ${CMAKE_CURRENT_SOURCE_DIR}/ada4250/iio_ada4250.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ada4250)

# adhv4710
no_os_sources_ifdef(CONFIG_AMPLIFIERS_ADHV4710 ${CMAKE_CURRENT_SOURCE_DIR}/adhv4710/adhv4710.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adhv4710)

# adl8113
no_os_sources_ifdef(CONFIG_AMPLIFIERS_ADL8113 ${CMAKE_CURRENT_SOURCE_DIR}/adl8113/adl8113.c)
no_os_sources_ifdef(CONFIG_AMPLIFIERS_IIO_ADL8113 ${CMAKE_CURRENT_SOURCE_DIR}/adl8113/iio_adl8113.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adl8113)
