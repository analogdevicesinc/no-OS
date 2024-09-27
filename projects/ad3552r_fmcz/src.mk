include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk
include $(PROJECT)/src/examples/examples_src.mk

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c

INCS += $(PROJECT)/src/common/common_data.h
SRCS += $(PROJECT)/src/common/common_data.c

INCS += $(PROJECT)/src/platform/platform_includes.h

INCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.h

SRCS += $(DRIVERS)/dac/ad3552r/ad3552r.c
INCS += $(DRIVERS)/dac/ad3552r/ad3552r.h

SRCS += $(NO-OS)/util/no_os_util.c \
        $(NO-OS)/util/no_os_alloc.c \
        $(NO-OS)/util/no_os_mutex.c \
        $(NO-OS)/util/no_os_sin_lut.c \
        $(NO-OS)/util/no_os_crc8.c \
	$(DRIVERS)/api/no_os_spi.c \
        $(DRIVERS)/api/no_os_gpio.c \
        $(NO-OS)/util/no_os_util.c

INCS += $(INCLUDE)/no_os_timer.h \
	$(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_crc8.h \
	$(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_mutex.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_axi_io.h

SRCS += $(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.c \
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.c \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c

INCS += $(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.h \
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.h \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h

INCS += $(PLATFORM_DRIVERS)/xilinx_gpio.h \
	$(PLATFORM_DRIVERS)/xilinx_spi.h

SRCS += $(PLATFORM_DRIVERS)/xilinx_axi_io.c \
	$(PLATFORM_DRIVERS)/xilinx_gpio.c \
	$(PLATFORM_DRIVERS)/xilinx_delay.c \
	$(PLATFORM_DRIVERS)/xilinx_spi.c
