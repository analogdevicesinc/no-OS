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

# Add to SRCS source files to be build in the project
SRCS += $(PROJECT)/src/ad7768_evb.c
SRCS += $(NO-OS)/util/no_os_fifo.c
SRCS += $(NO-OS)/util/no_os_util.c
SRCS += $(NO-OS)/util/no_os_list.c
SRCS += $(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c

# Add to INCS inlcude files to be build in the project
INCS += $(INCLUDE)/no_os_error.h
INCS += $(INCLUDE)/no_os_gpio.h
INCS += $(INCLUDE)/no_os_delay.h
INCS += $(INCLUDE)/no_os_util.h
INCS += $(INCLUDE)/no_os_axi_io.h
INCS += $(INCLUDE)/no_os_spi.h
INCS += $(INCLUDE)/no_os_timer.h
INCS += $(INCLUDE)/no_os_i2c.h
INCS += $(INCLUDE)/no_os_uart.h
INCS += $(INCLUDE)/no_os_lf256fifo.h
INCS +=	$(INCLUDE)/no_os_irq.h
INCS += $(INCLUDE)/no_os_list.h
INCS += $(INCLUDE)/no_os_fifo.h
INCS += $(INCLUDE)/no_os_alloc.h
INCS += $(PROJECT)/src/parameters.h \
	$(INCLUDE)/no_os_mutex.h \
	$(INCLUDE)/no_os_print_log.h

# Add to SRC_DIRS directories to be used in the build. All .c and .h files from
# the directory and subdirectories will be added to the build (recursively)
SRC_DIRS += $(PLATFORM_DRIVERS)
SRC_DIRS += $(DRIVERS)/adc/ad7768
SRC_DIRS += $(DRIVERS)/axi_core/axi_dmac
SRC_DIRS += $(DRIVERS)/axi_core/axi_adc_core

# Add to LIBRARIES the libraries that need to be linked in the build
# LIBRARIES += mqtt
# LIBRARIES += fatfs
# LIBRARIES += mbedtls
# LIBRARIES += iio

SRCS += $(DRIVERS)/api/no_os_gpio.c \
        $(DRIVERS)/api/no_os_i2c.c  \
        $(DRIVERS)/api/no_os_irq.c  \
	$(DRIVERS)/api/no_os_timer.c \
        $(DRIVERS)/api/no_os_spi.c

ifeq (y,$(strip $(IIOD)))
LIBRARIES += iio
SRC_DIRS += $(NO-OS)/iio/iio_app

INCS +=	$(INCLUDE)/no_os_fifo.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_list.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.h
SRCS += $(NO-OS)/util/no_os_fifo.c \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.c \
	$(NO-OS)/util/no_os_list.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c
endif
