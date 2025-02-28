INCS += $(PLATFORM_DRIVERS)/mbed_gpio.h			\
		$(PLATFORM_DRIVERS)/mbed_spi.h			\
		$(PLATFORM_DRIVERS)/mbed_gpio_irq.h		\
		$(PLATFORM_DRIVERS)/mbed_irq.h			\
		$(PLATFORM_DRIVERS)/mbed_uart.h		

SRCS += $(PLATFORM_DRIVERS)/mbed_delay.cpp		\
		$(PLATFORM_DRIVERS)/mbed_gpio.cpp		\
		$(PLATFORM_DRIVERS)/mbed_spi.cpp		\
		$(PLATFORM_DRIVERS)/mbed_gpio_irq.cpp	\
		$(PLATFORM_DRIVERS)/mbed_irq.cpp		\
		$(PLATFORM_DRIVERS)/mbed_uart.cpp
