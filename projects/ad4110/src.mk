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
SRCS += $(DRIVERS)/spi/spi.c						\
	$(DRIVERS)/irq/irq.c						\
	$(DRIVERS)/gpio/gpio.c						\
	$(DRIVERS)/afe/ad4110/ad4110.c
				
SRCS += $(PLATFORM_DRIVERS)/xilinx_spi.c				\
	$(PLATFORM_DRIVERS)/xilinx_irq.c				\
	$(PLATFORM_DRIVERS)/xilinx_gpio.c				\
	$(PLATFORM_DRIVERS)/xilinx_gpio_irq.c				\
	$(PLATFORM_DRIVERS)/delay.c					\
	$(NO-OS)/util/list.c

INCS += $(DRIVERS)/afe/ad4110/ad4110.h

INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h					\
	$(PLATFORM_DRIVERS)/irq_extra.h					\
	$(PLATFORM_DRIVERS)/gpio_extra.h				\
	$(PLATFORM_DRIVERS)/gpio_irq_extra.h

INCS += $(INCLUDE)/spi.h						\
	$(INCLUDE)/gpio.h						\
	$(INCLUDE)/error.h						\
	$(INCLUDE)/delay.h						\
	$(INCLUDE)/irq.h						\
	$(INCLUDE)/util.h						\
	$(INCLUDE)/print_log.h						\
	$(INCLUDE)/list.h
