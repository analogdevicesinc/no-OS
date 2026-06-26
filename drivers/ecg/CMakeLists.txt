# adas1000
no_os_sources_ifdef(CONFIG_ECG_ADAS1000 ${CMAKE_CURRENT_SOURCE_DIR}/adas1000/adas1000.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adas1000)
