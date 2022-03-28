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
SRCS += $(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/frequency/adf5902/adf5902.c
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c \
	$(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(PLATFORM_DRIVERS)/xilinx_gpio.c \
	$(PLATFORM_DRIVERS)/delay.c
ifeq (y,$(strip $(TINYIIOD)))
LIBRARIES += iio
SRCS += $(NO-OS)/util/no_os_fifo.c \
	$(NO-OS)/util/no_os_list.c \
	$(PLATFORM_DRIVERS)/no_os_uart.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
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
INCS +=	$(INCLUDE)/no_os_axi_io.h \
	$(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_print_log.h
ifeq (y,$(strip $(TINYIIOD)))
INCS += $(INCLUDE)/no_os_fifo.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_list.h \
	$(PLATFORM_DRIVERS)/irq_extra.h \
	$(PLATFORM_DRIVERS)/uart_extra.h \
	$(NO-OS)/iio/iio_app/iio_app.h
endif
