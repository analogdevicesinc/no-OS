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

SRCS += $(PROJECT)/src/ad469x_fmcz.c
SRCS += $(DRIVERS)/spi/spi.c						\
	$(DRIVERS)/gpio/gpio.c						\
	$(DRIVERS)/adc/ad469x/ad469x.c					\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c				\
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.c		\
	$(DRIVERS)/axi_core/axi_pwmgen/axi_pwm.c			\
	$(DRIVERS)/axi_core/spi_engine/spi_engine.c			\
	$(NO-OS)/util/util.c
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c					\
	$(PLATFORM_DRIVERS)/xilinx_gpio.c				\
	$(PLATFORM_DRIVERS)/xilinx_spi.c				\
	$(PLATFORM_DRIVERS)/delay.c
ifeq (y,$(strip $(TINYIIOD)))
LIBRARIES += iio
SRCS += $(PLATFORM_DRIVERS)/uart.c					\
	$(PLATFORM_DRIVERS)/irq.c					\
	$(NO-OS)/util/fifo.c						\
	$(NO-OS)/util/list.c						
endif
INCS += $(PROJECT)/src/parameters.h
INCS += $(DRIVERS)/adc/ad469x/ad469x.h					\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h				\
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.h		\
	$(DRIVERS)/axi_core/axi_pwmgen/axi_pwm_extra.h			\
	$(DRIVERS)/axi_core/spi_engine/spi_engine.h			\
	$(DRIVERS)/axi_core/spi_engine/spi_engine_private.h
INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h					\
	$(PLATFORM_DRIVERS)/irq_extra.h					\
	$(PLATFORM_DRIVERS)/uart_extra.h				\
	$(PLATFORM_DRIVERS)/gpio_extra.h
INCS +=	$(INCLUDE)/axi_io.h						\
	$(INCLUDE)/spi.h						\
	$(INCLUDE)/pwm.h						\
	$(INCLUDE)/gpio.h						\
	$(INCLUDE)/error.h						\
	$(INCLUDE)/delay.h						\
	$(INCLUDE)/irq.h						\
	$(INCLUDE)/uart.h						\
	$(INCLUDE)/util.h
ifeq (y,$(strip $(TINYIIOD)))
INCS += $(INCLUDE)/fifo.h						\
	$(INCLUDE)/irq.h						\
	$(INCLUDE)/uart.h						\
	$(INCLUDE)/list.h						\
	$(PLATFORM_DRIVERS)/irq_extra.h					\
	$(PLATFORM_DRIVERS)/uart_extra.h				
endif
