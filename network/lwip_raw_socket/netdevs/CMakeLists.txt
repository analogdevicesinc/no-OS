no_os_sources_ifdef(CONFIG_ADIN1110 ${CMAKE_CURRENT_SOURCE_DIR}/adin1110/lwip_adin1110.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adin1110)

no_os_sources_ifdef(CONFIG_ADIN1140 ${CMAKE_CURRENT_SOURCE_DIR}/adin1140/lwip_adin1140.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adin1140)
