# adg2404
no_os_sources_ifdef(CONFIG_MUX_ADG2404 ${CMAKE_CURRENT_SOURCE_DIR}/adg2404/adg2404.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adg2404)

# adgs1408
no_os_sources_ifdef(CONFIG_MUX_ADGS1408 ${CMAKE_CURRENT_SOURCE_DIR}/adgs1408/adgs1408.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adgs1408)

# adgs5412
no_os_sources_ifdef(CONFIG_MUX_ADGS5412 ${CMAKE_CURRENT_SOURCE_DIR}/adgs5412/adgs5412.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adgs5412)
