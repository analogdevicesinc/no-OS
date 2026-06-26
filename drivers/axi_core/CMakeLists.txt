# axi_adc_core
no_os_sources_ifdef(CONFIG_AXI_CORE_AXI_ADC_CORE ${CMAKE_CURRENT_SOURCE_DIR}/axi_adc_core/axi_adc_core.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/axi_adc_core)

# axi_dac_core
no_os_sources_ifdef(CONFIG_AXI_CORE_AXI_DAC_CORE ${CMAKE_CURRENT_SOURCE_DIR}/axi_dac_core/axi_dac_core.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/axi_dac_core)

# axi_dmac
no_os_sources_ifdef(CONFIG_AXI_CORE_AXI_DMAC ${CMAKE_CURRENT_SOURCE_DIR}/axi_dmac/axi_dmac.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/axi_dmac)

# axi_pwmgen
no_os_sources_ifdef(CONFIG_AXI_CORE_AXI_PWMGEN ${CMAKE_CURRENT_SOURCE_DIR}/axi_pwmgen/axi_pwm.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/axi_pwmgen)

# axi_sysid
no_os_sources_ifdef(CONFIG_AXI_CORE_AXI_SYSID ${CMAKE_CURRENT_SOURCE_DIR}/axi_sysid/axi_sysid.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/axi_sysid)

# clk_altera_a10_fpll
no_os_sources_ifdef(CONFIG_AXI_CORE_CLK_ALTERA_A10_FPLL
    ${CMAKE_CURRENT_SOURCE_DIR}/clk_altera_a10_fpll/clk_altera_a10_fpll.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/clk_altera_a10_fpll)

# clk_axi_clkgen
no_os_sources_ifdef(CONFIG_AXI_CORE_CLK_AXI_CLKGEN
    ${CMAKE_CURRENT_SOURCE_DIR}/clk_axi_clkgen/clk_axi_clkgen.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/clk_axi_clkgen)

# iio_axi_adc
no_os_sources_ifdef(CONFIG_AXI_CORE_IIO_AXI_ADC ${CMAKE_CURRENT_SOURCE_DIR}/iio_axi_adc/iio_axi_adc.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/iio_axi_adc)

# iio_axi_dac
no_os_sources_ifdef(CONFIG_AXI_CORE_IIO_AXI_DAC ${CMAKE_CURRENT_SOURCE_DIR}/iio_axi_dac/iio_axi_dac.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/iio_axi_dac)

# jesd204
no_os_sources_ifdef(CONFIG_AXI_CORE_JESD204
    ${CMAKE_CURRENT_SOURCE_DIR}/jesd204/altera_a10_atx_pll.c
    ${CMAKE_CURRENT_SOURCE_DIR}/jesd204/altera_a10_cdr_pll.c
    ${CMAKE_CURRENT_SOURCE_DIR}/jesd204/altera_adxcvr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/jesd204/axi_adxcvr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/jesd204/axi_jesd204_rx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/jesd204/axi_jesd204_tx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/jesd204/jesd204_clk.c
    ${CMAKE_CURRENT_SOURCE_DIR}/jesd204/xilinx_transceiver.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/jesd204)

# spi_engine
no_os_sources_ifdef(CONFIG_AXI_CORE_SPI_ENGINE ${CMAKE_CURRENT_SOURCE_DIR}/spi_engine/spi_engine.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/spi_engine)
