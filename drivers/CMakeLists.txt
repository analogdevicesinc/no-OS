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

if(CONFIG_AFE)
    add_subdirectory(afe)
endif()

if(CONFIG_AMPLIFIERS)
    add_subdirectory(amplifiers)
endif()

if(CONFIG_AXI_CORE)
    add_subdirectory(axi_core)
endif()

if(CONFIG_CDC)
    add_subdirectory(cdc)
endif()

if(CONFIG_CHARGER)
    add_subdirectory(charger)
endif()

if(CONFIG_DAC)
    add_subdirectory(dac)
endif()

if(CONFIG_DIGITAL_IO)
    add_subdirectory(digital-io)
endif()

if(CONFIG_DISPLAY)
    add_subdirectory(display)
endif()

if(CONFIG_ECG)
    add_subdirectory(ecg)
endif()

if(CONFIG_EEPROM_DRIVERS)
    add_subdirectory(eeprom)
endif()

if(CONFIG_FILTER)
    add_subdirectory(filter)
endif()

if(CONFIG_FREQUENCY)
    add_subdirectory(frequency)
endif()

if(CONFIG_GMSL)
    add_subdirectory(gmsl)
endif()

if(CONFIG_GYRO)
    add_subdirectory(gyro)
endif()

if(CONFIG_IMPEDANCE_ANALYZER)
    add_subdirectory(impedance-analyzer)
endif()

if(CONFIG_IMU)
    add_subdirectory(imu)
endif()

if(CONFIG_IO_EXPANDER)
    add_subdirectory(io-expander)
endif()

if(CONFIG_IO_LINK)
    add_subdirectory(io-link)
endif()

if(CONFIG_LED)
    add_subdirectory(led)
endif()

if(CONFIG_METER)
    add_subdirectory(meter)
endif()

if(CONFIG_MOTOR)
    add_subdirectory(motor)
endif()

if(CONFIG_MUX)
    add_subdirectory(mux)
endif()

if(CONFIG_NET)
    add_subdirectory(net)
endif()

if(CONFIG_PHOTO_ELECTRONIC)
    add_subdirectory(photo-electronic)
endif()

if(CONFIG_POTENTIOMETER)
    add_subdirectory(potentiometer)
endif()

if(CONFIG_POWER)
    add_subdirectory(power)
endif()

if(CONFIG_RESOLVER)
    add_subdirectory(resolver)
endif()

if(CONFIG_RF_TRANSCEIVER)
    add_subdirectory(rf-transceiver)
endif()

if(CONFIG_RTC_DRIVERS)
    add_subdirectory(rtc)
endif()

if(CONFIG_SD_CARD)
    add_subdirectory(sd-card)
endif()

if(CONFIG_SWITCH)
    add_subdirectory(switch)
endif()

if(CONFIG_TEMPERATURE)
    add_subdirectory(temperature)
endif()
