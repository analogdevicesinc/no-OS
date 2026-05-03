# adg1712
no_os_sources_ifdef(CONFIG_SWITCH_ADG1712 ${CMAKE_CURRENT_SOURCE_DIR}/adg1712/adg1712.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adg1712)

# adg1736
no_os_sources_ifdef(CONFIG_SWITCH_ADG1736 ${CMAKE_CURRENT_SOURCE_DIR}/adg1736/adg1736.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adg1736)

# adg2128
no_os_sources_ifdef(CONFIG_SWITCH_IIO_ADG2128 ${CMAKE_CURRENT_SOURCE_DIR}/adg2128/iio_adg2128.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adg2128)

# adgm3121
no_os_sources_ifdef(CONFIG_SWITCH_ADGM3121 ${CMAKE_CURRENT_SOURCE_DIR}/adgm3121/adgm3121.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adgm3121)

# adgs6414d
no_os_sources_ifdef(CONFIG_SWITCH_ADGS6414D ${CMAKE_CURRENT_SOURCE_DIR}/adgs6414d/adgs6414d.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adgs6414d)
