SRCS += $(PLATFORM_DRIVERS)/$(PLATFORM)_delay.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(DRIVERS)/api/no_os_timer.c

ifeq (y,$(strip $(NETWORKING)))
DISABLE_SECURE_SOCKET ?= y
SRC_DIRS += $(NO-OS)/network
endif

SRCS += $(NO-OS)/util/no_os_circular_buffer.c \
	$(PLATFORM_DRIVERS)/xilinx_timer.c

INCS += $(INCLUDE)/no_os_circular_buffer.h \
	$(INCLUDE)/no_os_timer.h           \
	$(PLATFORM_DRIVERS)/xilinx_timer.h  \
	$(PLATFORM_DRIVERS)/rtc_extra.h

SRCS += $(NO-OS)/util/no_os_lf256fifo.c  \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_spi.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_axi_io.c

INCS += $(INCLUDE)/no_os_rtc.h          \
	$(INCLUDE)/no_os_gpio.h         \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_spi.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.h

SRCS +=	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c \
	$(DRIVERS)/axi_core/spi_engine/spi_engine.c \
	$(DRIVERS)/axi_core/axi_pwmgen/axi_pwm.c \
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.c \

INCS += $(DRIVERS)/axi_core/axi_dmac/axi_dmac.h \
	$(DRIVERS)/axi_core/spi_engine/spi_engine.h \
	$(DRIVERS)/axi_core/spi_engine/spi_engine_private.h \
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.h \
	$(DRIVERS)/axi_core/axi_pwmgen/axi_pwm_extra.h
