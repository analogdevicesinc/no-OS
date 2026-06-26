# ade7753
no_os_sources_ifdef(CONFIG_METER_ADE7753 ${CMAKE_CURRENT_SOURCE_DIR}/ade7753/ade7753.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ade7753)

# ade7754
no_os_sources_ifdef(CONFIG_METER_ADE7754 ${CMAKE_CURRENT_SOURCE_DIR}/ade7754/ade7754.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ade7754)

# ade7758
no_os_sources_ifdef(CONFIG_METER_ADE7758 ${CMAKE_CURRENT_SOURCE_DIR}/ade7758/ade7758.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ade7758)

# ade7816 (SPI and I2C variants)
no_os_sources_ifdef(CONFIG_METER_ADE7816
    ${CMAKE_CURRENT_SOURCE_DIR}/ade7816/ade7816.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ade7816/ade7816_i2c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ade7816/ade7816_spi.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ade7816)

# ade7880
no_os_sources_ifdef(CONFIG_METER_ADE7880 ${CMAKE_CURRENT_SOURCE_DIR}/ade7880/ade7880.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ade7880)

# ade7913
no_os_sources_ifdef(CONFIG_METER_ADE7913 ${CMAKE_CURRENT_SOURCE_DIR}/ade7913/ade7913.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ade7913)

# ade7953
no_os_sources_ifdef(CONFIG_METER_ADE7953 ${CMAKE_CURRENT_SOURCE_DIR}/ade7953/ade7953.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ade7953)

# ade7978
no_os_sources_ifdef(CONFIG_METER_ADE7978 ${CMAKE_CURRENT_SOURCE_DIR}/ade7978/ade7978.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ade7978)

# ade9000
no_os_sources_ifdef(CONFIG_METER_ADE9000 ${CMAKE_CURRENT_SOURCE_DIR}/ade9000/ade9000.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ade9000)

# ade9078
no_os_sources_ifdef(CONFIG_METER_ADE9078 ${CMAKE_CURRENT_SOURCE_DIR}/ade9078/ade9078.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ade9078)

# ade9113
no_os_sources_ifdef(CONFIG_METER_ADE9113 ${CMAKE_CURRENT_SOURCE_DIR}/ade9113/ade9113.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ade9113)

# ade9153a
no_os_sources_ifdef(CONFIG_METER_ADE9153A ${CMAKE_CURRENT_SOURCE_DIR}/ade9153a/ade9153a.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ade9153a)

# ade9430
no_os_sources_ifdef(CONFIG_METER_ADE9430 ${CMAKE_CURRENT_SOURCE_DIR}/ade9430/ade9430.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ade9430)
