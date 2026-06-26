# ad9361
no_os_sources_ifdef(CONFIG_RF_TRANSCEIVER_AD9361
    ${CMAKE_CURRENT_SOURCE_DIR}/ad9361/ad9361.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ad9361/ad9361_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ad9361/ad9361_conv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ad9361/ad9361_util.c)
no_os_sources_ifdef(CONFIG_RF_TRANSCEIVER_IIO_AD9361 ${CMAKE_CURRENT_SOURCE_DIR}/ad9361/iio_ad9361.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad9361)

# adf7023
no_os_sources_ifdef(CONFIG_RF_TRANSCEIVER_ADF7023 ${CMAKE_CURRENT_SOURCE_DIR}/adf7023/adf7023.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adf7023)

# hmc630x
no_os_sources_ifdef(CONFIG_RF_TRANSCEIVER_HMC630X ${CMAKE_CURRENT_SOURCE_DIR}/hmc630x/hmc630x.c)
no_os_sources_ifdef(CONFIG_RF_TRANSCEIVER_IIO_HMC630X ${CMAKE_CURRENT_SOURCE_DIR}/hmc630x/iio_hmc630x.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/hmc630x)

# koror (adrv904x)
no_os_sources_ifdef(CONFIG_RF_TRANSCEIVER_ADRV904X
    ${CMAKE_CURRENT_SOURCE_DIR}/koror/adrv904x.c
    ${CMAKE_CURRENT_SOURCE_DIR}/koror/adrv904x_conv.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/koror)

# madura (adrv9025)
no_os_sources_ifdef(CONFIG_RF_TRANSCEIVER_ADRV9025
    ${CMAKE_CURRENT_SOURCE_DIR}/madura/adrv9025.c
    ${CMAKE_CURRENT_SOURCE_DIR}/madura/adrv9025_conv.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/madura)

# navassa (adrv9002)
no_os_sources_ifdef(CONFIG_RF_TRANSCEIVER_ADRV9002
    ${CMAKE_CURRENT_SOURCE_DIR}/navassa/adrv9002.c
    ${CMAKE_CURRENT_SOURCE_DIR}/navassa/adrv9002_conv.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/navassa)

# talise (adrv9009)
no_os_sources_ifdef(CONFIG_RF_TRANSCEIVER_ADRV9009
    ${CMAKE_CURRENT_SOURCE_DIR}/talise/adrv9009.c
    ${CMAKE_CURRENT_SOURCE_DIR}/talise/adrv9009_conv.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/talise)
