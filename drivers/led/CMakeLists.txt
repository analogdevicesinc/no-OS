# ltc3208
no_os_sources_ifdef(CONFIG_LED_LTC3208 ${CMAKE_CURRENT_SOURCE_DIR}/ltc3208/ltc3208.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ltc3208)

# ltc3220
no_os_sources_ifdef(CONFIG_LED_LTC3220 ${CMAKE_CURRENT_SOURCE_DIR}/ltc3220/ltc3220.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ltc3220)

# max25603
no_os_sources_ifdef(CONFIG_LED_MAX25603 ${CMAKE_CURRENT_SOURCE_DIR}/max25603/max25603.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/max25603)
