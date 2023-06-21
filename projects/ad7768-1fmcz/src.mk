################################################################################
#									       #
#     Shared variables:							       #
#	- PROJECT							       #
#	- DRIVERS							       #
#	- INCLUDE							       #
#	- PLATFORM_DRIVERS						       #
#	- NO-OS								       #
#									       #
################################################################################


SRC_DIRS += $(DRIVERS)/axi_core/clk_axi_clkgen

SRCS += $(PROJECT)/src/ad77681evb.c
SRCS += $(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/adc/ad7768-1/ad77681.c \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c \
	$(DRIVERS)/axi_core/spi_engine/spi_engine.c \
	$(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c
SRCS +=	$(PLATFORM_DRIVERS)/xilinx_axi_io.c \
	$(PLATFORM_DRIVERS)/xilinx_gpio.c \
	$(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(PLATFORM_DRIVERS)/xilinx_delay.c
INCS += $(PROJECT)/src/parameters.h
INCS += $(DRIVERS)/adc/ad7768-1/ad77681.h \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h \
	$(DRIVERS)/axi_core/spi_engine/spi_engine.h \
	$(DRIVERS)/axi_core/spi_engine/spi_engine_private.h
INCS +=	$(PLATFORM_DRIVERS)/$(PLATFORM)_spi.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.h
INCS +=	$(INCLUDE)/no_os_axi_io.h \
	$(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_mutex.h
