#See No-OS/tool/scripts/src_model.mk for variable description

SRC_DIRS += $(PROJECT)/src
SRC_DIRS += $(NO-OS)/iio/iio_app

# Add to SRCS source files to be build in the project
SRCS += $(NO-OS)/drivers/adc/ad7124/ad7124.c \
	$(NO-OS)/drivers/adc/ad7124/iio_ad7124.c \
	$(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_timer.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(DRIVERS)/api/no_os_irq.c

# Add to INCS inlcude files to be build in the porject
INCS += $(NO-OS)/drivers/adc/ad7124/ad7124.h \
	$(NO-OS)/drivers/adc/ad7124/iio_ad7124.h

SRC_DIRS += $(PLATFORM_DRIVERS)
SRC_DIRS += $(NO-OS)/util
SRC_DIRS += $(INCLUDE)

IIOD=y

