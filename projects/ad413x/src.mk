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
SRCS += $(DRIVERS)/api/spi.c \
	$(DRIVERS)/api/irq.c \
	$(DRIVERS)/api/gpio.c \
	$(DRIVERS)/afe/ad413x/ad413x.c
				
SRCS += $(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(PLATFORM_DRIVERS)/xilinx_irq.c \
	$(PLATFORM_DRIVERS)/xilinx_gpio.c \
	$(PLATFORM_DRIVERS)/xilinx_gpio_irq.c \
	$(PLATFORM_DRIVERS)/delay.c \
	$(NO-OS)/util/list.c

INCS += $(DRIVERS)/afe/ad413x/ad413x.h

INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h \
	$(PLATFORM_DRIVERS)/irq_extra.h \
	$(PLATFORM_DRIVERS)/gpio_extra.h \
	$(PLATFORM_DRIVERS)/gpio_irq_extra.h

INCS += $(INCLUDE)/no-os/spi.h \
	$(INCLUDE)/no-os/gpio.h \
	$(INCLUDE)/no-os/error.h \
	$(INCLUDE)/no-os/delay.h \
	$(INCLUDE)/no-os/irq.h \
	$(INCLUDE)/no-os/util.h \
	$(INCLUDE)/no-os/print_log.h \
	$(INCLUDE)/no-os/list.h
