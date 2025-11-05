# Core drivers - always needed
add_subdirectory(platform)
add_subdirectory(api)

# Conditional driver subdirectories based on menuconfig settings
if(CONFIG_ACCEL)
    add_subdirectory(accel)
endif()

if(CONFIG_ADC)
    add_subdirectory(adc)
endif()

if(CONFIG_ADC_DAC)
    add_subdirectory(adc-dac)
endif()

if(CONFIG_CHARGER)
    add_subdirectory(charger)
endif()

if(CONFIG_DAC)
    add_subdirectory(dac)
endif()

if(CONFIG_AFE)
    add_subdirectory(afe)
endif()

if(CONFIG_IMU)
    add_subdirectory(imu)
endif()

if(CONFIG_NET)
    add_subdirectory(net)
endif()

if(CONFIG_DISPLAY)
    add_subdirectory(display)
endif()

# add_subdirectory(accel)
# add_subdirectory(afe)
# add_subdirectory(amplifiers)
# add_subdirectory(axi_core)
# add_subdirectory(cdc)