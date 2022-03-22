include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk
include $(PROJECT)/src/examples/examples_src.mk

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c

INCS += $(PROJECT)/src/common/common_data.h
SRCS += $(PROJECT)/src/common/common_data.c
	
INCS += $(PROJECT)/src/platform/platform_includes.h

INCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.h
SRCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.c 

INCS += $(INCLUDE)/no_os_delay.h     \
		$(INCLUDE)/no_os_error.h     \
		$(INCLUDE)/no_os_gpio.h      \
		$(INCLUDE)/no_os_i2c.h       \
		$(INCLUDE)/no_os_print_log.h \
		$(INCLUDE)/no_os_spi.h       \
		$(INCLUDE)/no_os_uart.h      \
		$(INCLUDE)/no_os_util.h 

SRCS += $(DRIVERS)/api/no_os_gpio.c \
		$(DRIVERS)/api/no_os_i2c.c  \
		$(DRIVERS)/api/no_os_spi.c  \
		$(NO-OS)/util/no_os_util.c

INCS += $(DRIVERS)/accel/adxl355/adxl355.h
SRCS += $(DRIVERS)/accel/adxl355/adxl355.c

