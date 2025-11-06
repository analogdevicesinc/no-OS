add_subdirectory(${PLATFORM})

if(${CONFIG_FREERTOS})
        add_subdirectory(freeRTOS)
endif()

no_os_sources_ifdef(CONFIG_PLATFORM_I2C_BITBANG ${CMAKE_CURRENT_SOURCE_DIR}/i2c/i2c_bitbang.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/i2c)
