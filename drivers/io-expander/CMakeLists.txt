# adp5589
no_os_sources_ifdef(CONFIG_IO_EXPANDER_ADP5589 ${CMAKE_CURRENT_SOURCE_DIR}/adp5589/adp5589.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adp5589)

# demux_spi
no_os_sources_ifdef(CONFIG_IO_EXPANDER_DEMUX_SPI ${CMAKE_CURRENT_SOURCE_DIR}/demux_spi/demux_spi.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/demux_spi)

# ltc4306
no_os_sources_ifdef(CONFIG_IO_EXPANDER_LTC4306 ${CMAKE_CURRENT_SOURCE_DIR}/ltc4306/ltc4306.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ltc4306)

# ltc4332
no_os_sources_ifdef(CONFIG_IO_EXPANDER_LTC4332 ${CMAKE_CURRENT_SOURCE_DIR}/ltc4332/ltc4332.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ltc4332)
