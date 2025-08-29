if(${CONFIG_FREERTOS})
        include(FreeRTOS)
endif()

if(${CONFIG_LWIP})
        include(lwip)
endif()

if(${CONFIG_CORDIO})
        add_subdirectory(cordio)
endif()