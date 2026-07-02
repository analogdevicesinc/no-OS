no_os_sources_ifdef(CONFIG_ADIN1110 ${CMAKE_CURRENT_SOURCE_DIR}/adin1110/adin1110.c)
no_os_sources_ifdef(CONFIG_ADIN1110 ${CMAKE_CURRENT_SOURCE_DIR}/oa_tc6/oa_tc6.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adin1110)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/oa_tc6)

no_os_sources_ifdef(CONFIG_ADIN1320 ${CMAKE_CURRENT_SOURCE_DIR}/adin1320/adin1320.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adin1320)

no_os_sources_ifdef(CONFIG_MDIO_SPI ${CMAKE_CURRENT_SOURCE_DIR}/mdio_spi/mdio_spi.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mdio_spi)
