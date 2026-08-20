# Xilinx CAPI backends.
#
# Gated on the neutral CONFIG_CAPI_<MODULE> symbol only (like maxim/stm32 platform
# drivers). The PS-vs-PL variant selection is done inside each C source via #ifdef
# on the BSP-provided XPAR_*_NUM_INSTANCES macros, so on hardware that lacks a
# peripheral the preprocessor strips the whole body → no symbols → clean link.
# Both PS and PL variants of a module are listed; at most one compiles per .xsa.

# UART
no_os_sources_ifdef(CONFIG_CAPI_UART  ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_capi_uart_ps.c)
no_os_sources_ifdef(CONFIG_CAPI_UART  ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_capi_uart_pl_lite.c)
no_os_sources_ifdef(CONFIG_CAPI_UART  ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_capi_uart_pl_ns550.c)

# IRQ
no_os_sources_ifdef(CONFIG_CAPI_IRQ   ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_capi_irq_gic.c)
no_os_sources_ifdef(CONFIG_CAPI_IRQ   ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_capi_irq_intc.c)

# GPIO
no_os_sources_ifdef(CONFIG_CAPI_GPIO  ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_capi_gpio_ps.c)
no_os_sources_ifdef(CONFIG_CAPI_GPIO  ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_capi_gpio_pl.c)

# SPI
no_os_sources_ifdef(CONFIG_CAPI_SPI   ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_capi_spi_ps.c)
no_os_sources_ifdef(CONFIG_CAPI_SPI   ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_capi_spi_pl.c)

# TIMER
no_os_sources_ifdef(CONFIG_CAPI_TIMER ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_capi_timer_ps_ttc.c)
no_os_sources_ifdef(CONFIG_CAPI_TIMER ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_capi_timer_ps_scu.c)
no_os_sources_ifdef(CONFIG_CAPI_TIMER ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_capi_timer_pl.c)

# I2C
no_os_sources_ifdef(CONFIG_CAPI_I2C   ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_capi_i2c_ps.c)
no_os_sources_ifdef(CONFIG_CAPI_I2C   ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_capi_i2c_pl.c)

# alloc / time (no XPAR guard — always valid on the PS)
no_os_sources_ifdef(CONFIG_CAPI_ALLOC ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_capi_alloc.c)
no_os_sources_ifdef(CONFIG_CAPI_TIME  ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_capi_time.c)

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
