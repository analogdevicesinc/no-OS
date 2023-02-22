INCS += $(PLATFORM_DRIVERS)/mbed_uart.h      \
		$(PLATFORM_DRIVERS)/mbed_i2c.h       \
		$(PLATFORM_DRIVERS)/mbed_gpio.h      \
		$(PLATFORM_DRIVERS)/mbed_irq.h       \
		$(PLATFORM_DRIVERS)/mbed_gpio_irq.h  \
		$(PLATFORM_DRIVERS)/mbed_spi.h

SRCS += $(PLATFORM_DRIVERS)/mbed_uart.cpp       \
		$(PLATFORM_DRIVERS)/mbed_i2c.cpp        \
		$(PLATFORM_DRIVERS)/mbed_gpio.cpp       \
		$(PLATFORM_DRIVERS)/mbed_irq.cpp        \
		$(PLATFORM_DRIVERS)/mbed_gpio_irq.cpp   \
		$(PLATFORM_DRIVERS)/mbed_spi.cpp        \
		$(PLATFORM_DRIVERS)/mbed_delay.cpp
