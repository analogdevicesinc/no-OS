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

SRC_DIRS += $(PROJECT)/src
SRCS += $(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/afe/ad4110/ad4110.c
				
SRCS += $(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(PLATFORM_DRIVERS)/xilinx_irq.c \
	$(PLATFORM_DRIVERS)/xilinx_gpio.c \
	$(PLATFORM_DRIVERS)/xilinx_gpio_irq.c \
	$(PLATFORM_DRIVERS)/xilinx_delay.c \
	$(NO-OS)/util/no_os_list.c \
	$(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c

INCS += $(DRIVERS)/afe/ad4110/ad4110.h

INCS +=	$(PLATFORM_DRIVERS)/$(PLATFORM)_spi.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.h \
	$(PLATFORM_DRIVERS)/xilinx_gpio_irq.h

INCS += $(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_mutex.h \
	$(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_list.h
