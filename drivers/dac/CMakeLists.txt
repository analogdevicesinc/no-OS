# AD3530R
no_os_sources_ifdef(CONFIG_DAC_AD3530R ${CMAKE_CURRENT_SOURCE_DIR}/ad3530r/ad3530r.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad3530r)

# AD3552R
no_os_sources_ifdef(CONFIG_DAC_AD3552R ${CMAKE_CURRENT_SOURCE_DIR}/ad3552r/ad3552r.c)
no_os_sources_ifdef(CONFIG_DAC_IIO_AD3552R ${CMAKE_CURRENT_SOURCE_DIR}/ad3552r/iio_ad3552r.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad3552r)

# AD5421
no_os_sources_ifdef(CONFIG_DAC_AD5421 ${CMAKE_CURRENT_SOURCE_DIR}/ad5421/ad5421.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5421)

# AD5446
no_os_sources_ifdef(CONFIG_DAC_AD5446 ${CMAKE_CURRENT_SOURCE_DIR}/ad5446/ad5446.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5446)

# AD5449
no_os_sources_ifdef(CONFIG_DAC_AD5449 ${CMAKE_CURRENT_SOURCE_DIR}/ad5449/ad5449.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5449)

# AD5460
no_os_sources_ifdef(CONFIG_DAC_AD5460 ${CMAKE_CURRENT_SOURCE_DIR}/ad5460/ad5460.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5460)

# AD5628
no_os_sources_ifdef(CONFIG_DAC_AD5628 ${CMAKE_CURRENT_SOURCE_DIR}/ad5628/ad5628.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5628)

# AD5629R
no_os_sources_ifdef(CONFIG_DAC_AD5629R ${CMAKE_CURRENT_SOURCE_DIR}/ad5629r/ad5629r.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5629r)

# AD5686
no_os_sources_ifdef(CONFIG_DAC_AD5686 ${CMAKE_CURRENT_SOURCE_DIR}/ad5686/ad5686.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5686)

# AD5754R
no_os_sources_ifdef(CONFIG_DAC_AD5754R ${CMAKE_CURRENT_SOURCE_DIR}/ad5754r/ad5754r.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5754r)

# AD5755
no_os_sources_ifdef(CONFIG_DAC_AD5755 ${CMAKE_CURRENT_SOURCE_DIR}/ad5755/ad5755.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5755)

# AD5758
no_os_sources_ifdef(CONFIG_DAC_AD5758 ${CMAKE_CURRENT_SOURCE_DIR}/ad5758/ad5758.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5758)

# AD5761R
no_os_sources_ifdef(CONFIG_DAC_AD5761R ${CMAKE_CURRENT_SOURCE_DIR}/ad5761r/ad5761r.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5761r)

# AD5766
no_os_sources_ifdef(CONFIG_DAC_AD5766 ${CMAKE_CURRENT_SOURCE_DIR}/ad5766/ad5766.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5766)

# AD5770R
no_os_sources_ifdef(CONFIG_DAC_AD5770R ${CMAKE_CURRENT_SOURCE_DIR}/ad5770r/ad5770r.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5770r)

# AD5791
no_os_sources_ifdef(CONFIG_DAC_AD5791 ${CMAKE_CURRENT_SOURCE_DIR}/ad5791/ad5791.c)
no_os_sources_ifdef(CONFIG_DAC_IIO_AD5791 ${CMAKE_CURRENT_SOURCE_DIR}/ad5791/iio_ad5791.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5791)

# AD7293
no_os_sources_ifdef(CONFIG_DAC_AD7293 ${CMAKE_CURRENT_SOURCE_DIR}/ad7293/ad7293.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad7293)

# AD7303
no_os_sources_ifdef(CONFIG_DAC_AD7303 ${CMAKE_CURRENT_SOURCE_DIR}/ad7303/ad7303.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad7303)

# AD8460
no_os_sources_ifdef(CONFIG_DAC_AD8460 ${CMAKE_CURRENT_SOURCE_DIR}/ad8460/ad8460.c)
no_os_sources_ifdef(CONFIG_DAC_IIO_AD8460 ${CMAKE_CURRENT_SOURCE_DIR}/ad8460/iio_ad8460.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad8460)

# AD9144
no_os_sources_ifdef(CONFIG_DAC_AD9144 ${CMAKE_CURRENT_SOURCE_DIR}/ad9144/ad9144.c)
no_os_sources_ifdef(CONFIG_DAC_IIO_AD9144 ${CMAKE_CURRENT_SOURCE_DIR}/ad9144/iio_ad9144.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad9144)

# AD9152
no_os_sources_ifdef(CONFIG_DAC_AD9152 ${CMAKE_CURRENT_SOURCE_DIR}/ad9152/ad9152.c)
no_os_sources_ifdef(CONFIG_DAC_IIO_AD9152 ${CMAKE_CURRENT_SOURCE_DIR}/ad9152/iio_ad9152.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad9152)

# AD917X
no_os_sources_ifdef(CONFIG_DAC_AD917X ${CMAKE_CURRENT_SOURCE_DIR}/ad917x/ad917x.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad917x)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad917x/ad917x_api)

# AD9739A
no_os_sources_ifdef(CONFIG_DAC_AD9739A ${CMAKE_CURRENT_SOURCE_DIR}/ad9739a/ad9739a.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad9739a)

# DAC Demo
no_os_sources_ifdef(CONFIG_DAC_DEMO ${CMAKE_CURRENT_SOURCE_DIR}/dac_demo/dac_demo.c)
no_os_sources_ifdef(CONFIG_DAC_IIO_DEMO ${CMAKE_CURRENT_SOURCE_DIR}/dac_demo/iio_dac_demo.c)
no_os_sources_ifdef(CONFIG_DAC_IIO_DEMO ${CMAKE_CURRENT_SOURCE_DIR}/dac_demo/iio_dac_demo_trig.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/dac_demo)

# LTC2672
no_os_sources_ifdef(CONFIG_DAC_LTC2672 ${CMAKE_CURRENT_SOURCE_DIR}/ltc2672/ltc2672.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ltc2672)

# LTC268X
no_os_sources_ifdef(CONFIG_DAC_LTC268X ${CMAKE_CURRENT_SOURCE_DIR}/ltc268x/ltc268x.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ltc268x)

# MAX22017
no_os_sources_ifdef(CONFIG_DAC_MAX22017 ${CMAKE_CURRENT_SOURCE_DIR}/max22017/max22017.c)
no_os_sources_ifdef(CONFIG_DAC_IIO_MAX22017 ${CMAKE_CURRENT_SOURCE_DIR}/max22017/iio_max22017.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/max22017)

# MAX538X
no_os_sources_ifdef(CONFIG_DAC_MAX538X ${CMAKE_CURRENT_SOURCE_DIR}/max538x/max538x.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/max538x)