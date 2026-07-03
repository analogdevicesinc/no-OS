no_os_sources_ifdef(CONFIG_ADIN1110 ${CMAKE_CURRENT_SOURCE_DIR}/adin1110/adin1110.c)
no_os_sources_ifdef(CONFIG_ADIN1110 ${CMAKE_CURRENT_SOURCE_DIR}/oa_tc6/oa_tc6.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adin1110)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/oa_tc6)

no_os_sources_ifdef(CONFIG_ADIN1320 ${CMAKE_CURRENT_SOURCE_DIR}/adin1320/adin1320.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adin1320)

no_os_sources_ifdef(CONFIG_NET_ADIN1300 ${CMAKE_CURRENT_SOURCE_DIR}/adin1300.c)
no_os_sources_ifdef(CONFIG_NET_IIO_ADIN1300 ${CMAKE_CURRENT_SOURCE_DIR}/iio_adin1300.c)

no_os_sources_ifdef(CONFIG_NET_MAX24287 ${CMAKE_CURRENT_SOURCE_DIR}/max24287.c)
no_os_sources_ifdef(CONFIG_NET_IIO_MAX24287 ${CMAKE_CURRENT_SOURCE_DIR}/iio_max24287.c)

no_os_sources_ifdef(CONFIG_NET_MDIO_BITBANG ${CMAKE_CURRENT_SOURCE_DIR}/mdio_bitbang.c)

if(CONFIG_NET_ADIN1300 OR CONFIG_NET_MAX24287 OR CONFIG_NET_MDIO_BITBANG)
    target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()

no_os_sources_ifdef(CONFIG_MDIO_SPI ${CMAKE_CURRENT_SOURCE_DIR}/mdio_spi/mdio_spi.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mdio_spi)
