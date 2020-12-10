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

# Add to INCS inlcude files to be build in the porject
INCS += $(INCLUDE)/error.h
INCS += $(INCLUDE)/gpio.h
INCS += $(INCLUDE)/delay.h
INCS += $(INCLUDE)/util.h
INCS += $(INCLUDE)/axi_io.h
INCS += $(INCLUDE)/spi.h
INCS += $(INCLUDE)/timer.h
INCS += $(INCLUDE)/i2c.h
INCS += $(INCLUDE)/list.h
INCS += $(INCLUDE)/uart.h
INCS += $(INCLUDE)/irq.h
INCS += $(INCLUDE)/fifo.h
INCS += $(PROJECT)/src/parameters.h

# Add to SRC_DIRS directories to be used in the build. All .c and .h files from
# the directory and subdirectories will be added to the build (recursively)
SRC_DIRS += $(PLATFORM_DRIVERS)
SRC_DIRS += $(DRIVERS)/adc/ad7768/
SRC_DIRS += $(DRIVERS)/axi_core/axi_dmac
SRC_DIRS += $(DRIVERS)/axi_core/axi_adc_core
SRC_DIRS += $(DRIVERS)/spi
SRC_DIRS += $(DRIVERS)/i2c
SRC_DIRS += $(DRIVERS)/gpio

# Add to LIBRARIES the libraries that need to be linked in the build
# LIBRARIES += mqtt
# LIBRARIES += fatfs
# LIBRARIES += mbedtls
# LIBRARIES += iio

ifeq (y,$(strip $(TINYIIOD)))
LIBRARIES += iio
INCS +=	$(PROJECT)/src/app_iio.h					\
	$(INCLUDE)/xml.h						\
	$(INCLUDE)/fifo.h						\
	$(INCLUDE)/irq.h						\
	$(INCLUDE)/uart.h						\
	$(INCLUDE)/list.h						\
	$(PLATFORM_DRIVERS)/irq_extra.h					\
	$(PLATFORM_DRIVERS)/uart_extra.h                                \
	$(NO-OS)/iio/iio_axi_adc/iio_axi_adc.h
SRCS += $(PROJECT)/src/app_iio.c					\
	$(NO-OS)/util/xml.c						\
	$(NO-OS)/util/fifo.c						\
	$(NO-OS)/iio/iio_axi_adc/iio_axi_adc.c				\
	$(NO-OS)/util/list.c						\
	$(PLATFORM_DRIVERS)/uart.c					\
	$(PLATFORM_DRIVERS)/irq.c
endif
