no_os_sources_ifdef(CONFIG_ADIN1110 ${CMAKE_CURRENT_SOURCE_DIR}/adin1110/adin1110.c)
no_os_sources_ifdef(CONFIG_ADIN1110 ${CMAKE_CURRENT_SOURCE_DIR}/oa_tc6/oa_tc6.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adin1110)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/oa_tc6)