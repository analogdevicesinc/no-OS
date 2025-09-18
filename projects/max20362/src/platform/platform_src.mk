# Platform-specific parameters
INCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.h
SRCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.c

# Platform driver includes
INCS += $(PLATFORM_DRIVERS)/../common/$(PLATFORM)_dma.h		\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.h					\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_i2c.h					\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.h					\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h					\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart_stdio.h			\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_timer.h

# Platform driver sources
SRCS += $(PLATFORM_DRIVERS)/../common/$(PLATFORM)_dma.c 	\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.c					\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_delay.c					\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_i2c.c					\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c					\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.c					\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart_stdio.c			\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_timer.c
