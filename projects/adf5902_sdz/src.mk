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

SRCS += $(PROJECT)/src/adf5902_sdz.c
SRCS += $(DRIVERS)/api/spi.c \
	$(DRIVERS)/api/gpio.c \
	$(DRIVERS)/frequency/adf5902/adf5902.c
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c \
	$(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(PLATFORM_DRIVERS)/xilinx_gpio.c \
	$(PLATFORM_DRIVERS)/delay.c
ifeq (y,$(strip $(TINYIIOD)))
LIBRARIES += iio
SRCS += $(NO-OS)/util/fifo.c \
	$(NO-OS)/util/list.c \
	$(PLATFORM_DRIVERS)/uart.c \
	$(PLATFORM_DRIVERS)/irq.c \
	$(NO-OS)/iio/iio_app/iio_app.c \
	$(DRIVERS)/frequency/adf5902/iio_adf5902.c
endif
INCS +=	$(PROJECT)/src/app_config.h \
	$(PROJECT)/src/parameters.h
ifeq (y,$(strip $(TINYIIOD)))
INCS +=	$(DRIVERS)/frequency/adf5902/iio_adf5902.h
endif
INCS += $(DRIVERS)/frequency/adf5902/adf5902.h
INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h \
	$(PLATFORM_DRIVERS)/gpio_extra.h
INCS +=	$(INCLUDE)/no-os/axi_io.h \
	$(INCLUDE)/no-os/spi.h \
	$(INCLUDE)/no-os/gpio.h \
	$(INCLUDE)/no-os/error.h \
	$(INCLUDE)/no-os/delay.h \
	$(INCLUDE)/no-os/util.h \
	$(INCLUDE)/no-os/print_log.h
ifeq (y,$(strip $(TINYIIOD)))
INCS += $(INCLUDE)/no-os/fifo.h \
	$(INCLUDE)/no-os/irq.h \
	$(INCLUDE)/no-os/uart.h \
	$(INCLUDE)/no-os/list.h \
	$(PLATFORM_DRIVERS)/irq_extra.h \
	$(PLATFORM_DRIVERS)/uart_extra.h \
	$(NO-OS)/iio/iio_app/iio_app.h
endif
