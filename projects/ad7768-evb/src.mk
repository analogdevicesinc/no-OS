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
SRCS += $(NO-OS)/util/fifo.c
SRCS += $(NO-OS)/util/util.c
SRCS += $(NO-OS)/util/list.c

# Add to INCS inlcude files to be build in the project
INCS += $(INCLUDE)/no-os/error.h
INCS += $(INCLUDE)/no-os/gpio.h
INCS += $(INCLUDE)/no-os/delay.h
INCS += $(INCLUDE)/no-os/util.h
INCS += $(INCLUDE)/no-os/axi_io.h
INCS += $(INCLUDE)/no-os/spi.h
INCS += $(INCLUDE)/no-os/timer.h
INCS += $(INCLUDE)/no-os/i2c.h
INCS += $(INCLUDE)/no-os/uart.h
INCS +=	$(INCLUDE)/no-os/irq.h
INCS += $(INCLUDE)/no-os/list.h
INCS += $(INCLUDE)/no-os/fifo.h
INCS += $(PROJECT)/src/parameters.h

# Add to SRC_DIRS directories to be used in the build. All .c and .h files from
# the directory and subdirectories will be added to the build (recursively)
SRC_DIRS += $(PLATFORM_DRIVERS)
SRC_DIRS += $(DRIVERS)/adc/ad7768
SRC_DIRS += $(DRIVERS)/axi_core/axi_dmac
SRC_DIRS += $(DRIVERS)/axi_core/axi_adc_core
SRC_DIRS += $(DRIVERS)/api

# Add to LIBRARIES the libraries that need to be linked in the build
# LIBRARIES += mqtt
# LIBRARIES += fatfs
# LIBRARIES += mbedtls
# LIBRARIES += iio

ifeq (y,$(strip $(TINYIIOD)))
LIBRARIES += iio
SRC_DIRS += $(NO-OS)/iio/iio_app

INCS +=	$(INCLUDE)/no-os/fifo.h \
	$(INCLUDE)/no-os/uart.h \
	$(INCLUDE)/no-os/list.h \
	$(PLATFORM_DRIVERS)/irq_extra.h \
	$(PLATFORM_DRIVERS)/uart_extra.h \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.h
SRCS += $(NO-OS)/util/fifo.c \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.c \
	$(NO-OS)/util/list.c \
	$(PLATFORM_DRIVERS)/uart.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c
endif
