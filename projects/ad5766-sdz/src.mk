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

SRCS += $(PROJECT)/src/ad5766_sdz.c
SRCS += $(PROJECT)/src/ad5766_core.c \
	$(DRIVERS)/dac/ad5766/ad5766.c \
	$(DRIVERS)/api/spi.c \
	$(DRIVERS)/api/gpio.c \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c \
	$(DRIVERS)/axi_core/spi_engine/spi_engine.c \
	$(NO-OS)/util/util.c
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c \
	$(PLATFORM_DRIVERS)/xilinx_gpio.c \
	$(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(PLATFORM_DRIVERS)/delay.c
INCS += $(PROJECT)/src/parameters.h \
	$(PROJECT)/src/ad5766_core.h \
	$(DRIVERS)/dac/ad5766/ad5766.h \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h \
	$(DRIVERS)/axi_core/spi_engine/spi_engine.h \
	$(DRIVERS)/axi_core/spi_engine/spi_engine_private.h
INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h \
	$(PLATFORM_DRIVERS)/gpio_extra.h
INCS +=	$(INCLUDE)/no-os/axi_io.h \
	$(INCLUDE)/no-os/spi.h \
	$(INCLUDE)/no-os/gpio.h \
	$(INCLUDE)/no-os/error.h \
	$(INCLUDE)/no-os/delay.h \
	$(INCLUDE)/no-os/print_log.h \
	$(INCLUDE)/no-os/util.h
