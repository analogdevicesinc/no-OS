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

SRCS += $(PROJECT)/src/adaq7980_sdz.c
SRCS += $(DRIVERS)/spi/spi.c \
	$(DRIVERS)/gpio/gpio.c \
	$(DRIVERS)/adc/adaq7980/adaq7980.c \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c \
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.c \
	$(DRIVERS)/axi_core/axi_pwmgen/axi_pwm.c \
	$(DRIVERS)/axi_core/spi_engine/spi_engine.c \
	$(NO-OS)/util/util.c
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c \
	$(PLATFORM_DRIVERS)/xilinx_gpio.c \
	$(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(PLATFORM_DRIVERS)/delay.c
INCS += $(PROJECT)/src/parameters.h
INCS += $(DRIVERS)/adc/adaq7980/adaq7980.h \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h \
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.h \
	$(DRIVERS)/axi_core/axi_pwmgen/axi_pwm_extra.h \
	$(DRIVERS)/axi_core/spi_engine/spi_engine.h \
	$(DRIVERS)/axi_core/spi_engine/spi_engine_private.h
INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h \
	$(PLATFORM_DRIVERS)/irq_extra.h \
	$(PLATFORM_DRIVERS)/uart_extra.h \
	$(PLATFORM_DRIVERS)/gpio_extra.h
INCS +=	$(INCLUDE)/axi_io.h \
	$(INCLUDE)/spi.h \
	$(INCLUDE)/pwm.h \
	$(INCLUDE)/gpio.h \
	$(INCLUDE)/error.h \
	$(INCLUDE)/delay.h \
	$(INCLUDE)/irq.h \
	$(INCLUDE)/uart.h \
	$(INCLUDE)/util.h
