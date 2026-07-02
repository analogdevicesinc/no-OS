no_os_sources_ifdef(CONFIG_DISPLAY ${CMAKE_CURRENT_SOURCE_DIR}/display.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

no_os_sources_ifdef(CONFIG_DISPLAY_NHD_C12832A1Z ${CMAKE_CURRENT_SOURCE_DIR}/nhd_c12832a1z/nhd_c12832a1z.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/nhd_c12832a1z)

no_os_sources_ifdef(CONFIG_DISPLAY_SSD1306 ${CMAKE_CURRENT_SOURCE_DIR}/ssd_1306/ssd_1306.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ssd_1306)
