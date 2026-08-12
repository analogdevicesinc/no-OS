# max22516
no_os_sources_ifdef(CONFIG_IO_LINK_MAX22516 ${CMAKE_CURRENT_SOURCE_DIR}/max22516/max22516.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/max22516)

# adiol100
no_os_sources_ifdef(CONFIG_IO_LINK_ADIOL100 ${CMAKE_CURRENT_SOURCE_DIR}/adiol100/adiol100.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adiol100)
