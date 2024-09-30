INCS += $(PLATFORM_DRIVERS)/stm32_gpio.h		\
	$(PLATFORM_DRIVERS)/stm32_spi.h			\
	$(PLATFORM_DRIVERS)/stm32_irq.h			\
	$(PLATFORM_DRIVERS)/stm32_uart.h		\
	$(PLATFORM_DRIVERS)/stm32_hal.h			\
	$(PLATFORM_DRIVERS)/stm32_pwm.h			\
	$(PLATFORM_DRIVERS)/stm32_timer.h

SRCS += $(PLATFORM_DRIVERS)/stm32_gpio.c		\
	$(PLATFORM_DRIVERS)/stm32_uart.c		\
	$(PLATFORM_DRIVERS)/stm32_irq.c			\
	$(PLATFORM_DRIVERS)/stm32_pwm.c			\
	$(PLATFORM_DRIVERS)/stm32_timer.c
