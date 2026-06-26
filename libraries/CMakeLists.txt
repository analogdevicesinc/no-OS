if(${CONFIG_FREERTOS})
        include(FreeRTOS)
endif()

if(${CONFIG_LWIP})
        include(lwip)
endif()

if(${CONFIG_CORDIO})
        add_subdirectory(cordio)
endif()

if(${CONFIG_MBEDTLS})
        include(mbedtls)
endif()

if(${CONFIG_MQTT})
        include(mqtt)
endif()

if(${CONFIG_ESH})
        include(esh)
endif()

if(${CONFIG_LVGL})
        include(lvgl)
endif()

if(${CONFIG_AZURE})
        include(azure)
endif()
