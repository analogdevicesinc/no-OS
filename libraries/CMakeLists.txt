if(${CONFIG_FREERTOS})
        include(FreeRTOS)
endif()

if(${CONFIG_LWIP})
        include(lwip)
endif()