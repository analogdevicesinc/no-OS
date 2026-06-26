# sd
no_os_sources_ifdef(CONFIG_SD_CARD_SD ${CMAKE_CURRENT_SOURCE_DIR}/sd/sd.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/sd)
