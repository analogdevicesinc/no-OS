SRCS += $(PLATFORM_DRIVERS)/$(PLATFORM)_spi.c		\
		$(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.c 		\
		$(PLATFORM_DRIVERS)/$(PLATFORM)_delay.c 	\
		$(PLATFORM_DRIVERS)/$(PLATFORM)_axi_io.c	\
		$(DRIVERS)/axi_core/jesd204/axi_adxcvr.c 	\
		$(DRIVERS)/axi_core/jesd204/$(PLATFORM)_transceiver.c

INCS +=	$(PLATFORM_DRIVERS)/$(PLATFORM)_spi.h 		\
		$(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.h	\
		$(DRIVERS)/axi_core/jesd204/axi_adxcvr.h \
		$(DRIVERS)/axi_core/jesd204/$(PLATFORM)_transceiver.h 	\
	
ifeq (y,$(strip $(TINYIIOD)))
SRCS += $(PLATFORM_DRIVERS)/$(PLATFORM)_uart.c 		\
		$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c
endif

ifeq (y,$(strip $(TINYIIOD)))
SRCS += $(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h 		\
		$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.h
endif