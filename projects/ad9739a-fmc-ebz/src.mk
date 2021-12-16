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

SRCS := $(PROJECT)/src/ad9739a_fmc_ebz.c
ifeq (y,$(strip $(TINYIIOD)))
LIBRARIES += iio
SRC_DIRS += $(NO-OS)/iio/iio_app
endif
SRCS += $(DRIVERS)/dac/ad9739a/ad9739a.c \
	$(DRIVERS)/frequency/adf4350/adf4350.c \
	$(DRIVERS)/api/spi.c \
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.c \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c \
	$(NO-OS)/util/util.c
ifeq (y,$(strip $(TINYIIOD)))
SRCS += $(NO-OS)/util/fifo.c \
	$(DRIVERS)/axi_core/iio_axi_dac/iio_axi_dac.c \
	$(DRIVERS)/api/irq.c \
	$(NO-OS)/util/list.c \
	$(PLATFORM_DRIVERS)/uart.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c
endif
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c \
	$(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(PLATFORM_DRIVERS)/delay.c
INCS += $(PROJECT)/src/parameters.h \
	$(DRIVERS)/frequency/adf4350/adf4350.h \
	$(DRIVERS)/dac/ad9739a/ad9739a.h \
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.h \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h
INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h					
INCS +=	$(INCLUDE)/no-os/axi_io.h \
	$(INCLUDE)/no-os/spi.h \
	$(INCLUDE)/no-os/error.h \
	$(INCLUDE)/no-os/delay.h \
	$(INCLUDE)/no-os/print_log.h \
	$(INCLUDE)/no-os/util.h
ifeq (y,$(strip $(TINYIIOD)))
INCS +=	$(INCLUDE)/no-os/fifo.h \
	$(INCLUDE)/no-os/irq.h \
	$(INCLUDE)/no-os/uart.h \
	$(INCLUDE)/no-os/list.h \
	$(PLATFORM_DRIVERS)/irq_extra.h \
	$(PLATFORM_DRIVERS)/uart_extra.h \
	$(DRIVERS)/axi_core/iio_axi_dac/iio_axi_dac.h
endif
