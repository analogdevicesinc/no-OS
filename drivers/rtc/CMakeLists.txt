# max31343
no_os_sources_ifdef(CONFIG_RTC_DRIVERS_MAX31343 ${CMAKE_CURRENT_SOURCE_DIR}/max31343/max31343.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/max31343)

# pcf85263
no_os_sources_ifdef(CONFIG_RTC_DRIVERS_PCF85263 ${CMAKE_CURRENT_SOURCE_DIR}/pcf85263/pcf85263.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/pcf85263)
