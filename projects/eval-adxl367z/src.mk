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
include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk
include $(PROJECT)/src/examples/examples_src.mk

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c

INCS += $(PROJECT)/src/common/common_data.h
SRCS += $(PROJECT)/src/common/common_data.c

INCS += $(PROJECT)/src/platform/platform_includes.h

INCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.h
SRCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.c 


SRCS += $(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_i2c.c \
	$(DRIVERS)/accel/adxl367/adxl367.c \
	$(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_list.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c

INCS += $(DRIVERS)/accel/adxl367/adxl367.h

INCS += $(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_i2c.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_list.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_mutex.h
