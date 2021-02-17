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
ifeq (y,$(strip $(TINYIIOD)))
SRCS += $(PROJECT)/src/app_iio.c
endif
SRCS += $(DRIVERS)/spi/spi.c						\
	$(DRIVERS)/gpio/gpio.c							\
	$(DRIVERS)/frequency/adf5902/adf5902.c
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c				\
	$(PLATFORM_DRIVERS)/xilinx_spi.c				\
	$(PLATFORM_DRIVERS)/xilinx_gpio.c				\
	$(PLATFORM_DRIVERS)/delay.c
ifeq (y,$(strip $(TINYIIOD)))
LIBRARIES += iio
SRCS += $(NO-OS)/util/xml.c						\
	$(NO-OS)/util/fifo.c						\
	$(NO-OS)/util/list.c						\
	$(PLATFORM_DRIVERS)/uart.c					\
	$(PLATFORM_DRIVERS)/irq.c
endif
INCS +=	$(PROJECT)/src/app_config.h					\
	$(PROJECT)/src/parameters.h
ifeq (y,$(strip $(TINYIIOD)))
INCS +=	$(PROJECT)/src/app_iio.h					\
	$(DRIVERS)/frequency/adf5902/iio_adf5902.h
endif
INCS += $(DRIVERS)/frequency/adf5902/adf5902.h
INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h				\
	$(PLATFORM_DRIVERS)/gpio_extra.h
INCS +=	$(INCLUDE)/axi_io.h							\
	$(INCLUDE)/spi.h								\
	$(INCLUDE)/gpio.h								\
	$(INCLUDE)/error.h								\
	$(INCLUDE)/delay.h								\
	$(INCLUDE)/util.h								\
	$(INCLUDE)/print_log.h
ifeq (y,$(strip $(TINYIIOD)))
INCS += $(INCLUDE)/xml.h						\
	$(INCLUDE)/fifo.h						\
	$(INCLUDE)/irq.h						\
	$(INCLUDE)/uart.h						\
	$(INCLUDE)/list.h						\
	$(PLATFORM_DRIVERS)/irq_extra.h					\
	$(PLATFORM_DRIVERS)/uart_extra.h
endif
