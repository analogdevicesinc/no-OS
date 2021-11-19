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

SRCS := $(PROJECT)/src/ad9265_fmc_125ebz.c
ifeq (y,$(strip $(TINYIIOD)))
LIBRARIES += iio
SRCS += $(PROJECT)/src/app_iio.c
endif
SRCS += $(DRIVERS)/adc/ad9265/ad9265.c \
	$(DRIVERS)/spi/spi.c \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c \
	$(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c \
	$(NO-OS)/util/util.c
ifeq (y,$(strip $(TINYIIOD)))
SRCS += $(NO-OS)/util/fifo.c \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.c \
	$(DRIVERS)/irq/irq.c \
	$(NO-OS)/util/list.c \
	$(PLATFORM_DRIVERS)/uart.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c
endif
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c \
	$(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(PLATFORM_DRIVERS)/delay.c
INCS += $(PROJECT)/src/parameters.h \
	$(DRIVERS)/adc/ad9265/ad9265.h \
	$(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h
ifeq (y,$(strip $(TINYIIOD)))
INCS +=	$(PROJECT)/src/app_iio.h
endif
INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h
INCS +=	$(INCLUDE)/axi_io.h \
	$(INCLUDE)/spi.h \
	$(INCLUDE)/error.h \
	$(INCLUDE)/delay.h \
	$(INCLUDE)/print_log.h \
	$(INCLUDE)/util.h
ifeq (y,$(strip $(TINYIIOD)))
INCS +=	$(INCLUDE)/fifo.h \
	$(INCLUDE)/irq.h \
	$(INCLUDE)/uart.h \
	$(INCLUDE)/list.h \
	$(PLATFORM_DRIVERS)/irq_extra.h \
	$(PLATFORM_DRIVERS)/uart_extra.h \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.h
endif
