add_subdirectory(${PLATFORM})

if(${CONFIG_FREERTOS})
        add_subdirectory(freeRTOS)
endif()

no_os_sources_ifdef(CONFIG_PLATFORM_I2C_BITBANG ${CMAKE_CURRENT_SOURCE_DIR}/i2c/bitbang_i2c.c)
no_os_sources_ifdef(CONFIG_PLATFORM_I2C_BITBANG_CAPI ${CMAKE_CURRENT_SOURCE_DIR}/i2c/capi_i2c_bitbang.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/i2c)
