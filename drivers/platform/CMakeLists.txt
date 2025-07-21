add_subdirectory(${PLATFORM})

if(${CONFIG_FREERTOS})
        add_subdirectory(freeRTOS)
endif()