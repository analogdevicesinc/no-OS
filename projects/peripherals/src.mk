
SRCS += $(PROJECT)/src/example.c

# Common no_os headers needed by CAPI drivers
INCS += $(INCLUDE)/no_os_error.h	\
	$(INCLUDE)/no_os_alloc.h	\
	$(INCLUDE)/no_os_util.h		\
	$(INCLUDE)/no_os_delay.h	\
	$(INCLUDE)/no_os_pwm.h		\
	$(INCLUDE)/no_os_gpio.h		\
	$(INCLUDE)/no_os_irq.h		\
	$(INCLUDE)/no_os_mutex.h	\
	$(INCLUDE)/no_os_lf256fifo.h	\
	$(INCLUDE)/no_os_list.h		\
	$(PLATFORM_DRIVERS)/stm32_hal.h	\
	$(PLATFORM_DRIVERS)/stm32_pwm.h	\
	$(PLATFORM_DRIVERS)/stm32_gpio.h	\
	$(PLATFORM_DRIVERS)/stm32_irq.h

# Common no_os sources needed by CAPI drivers
SRCS += $(NO-OS)/util/no_os_alloc.c	\
	$(NO-OS)/util/no_os_mutex.c	\
	$(NO-OS)/util/no_os_lf256fifo.c	\
	$(NO-OS)/util/no_os_list.c	\
	$(PLATFORM_DRIVERS)/stm32_delay.c	\
	$(DRIVERS)/api/no_os_pwm.c	\
	$(DRIVERS)/api/no_os_gpio.c	\
	$(DRIVERS)/api/no_os_irq.c	\
	$(PLATFORM_DRIVERS)/stm32_pwm.c	\
	$(PLATFORM_DRIVERS)/stm32_gpio.c	\
	$(PLATFORM_DRIVERS)/stm32_irq.c

# CAPI headers
INCS += $(INCLUDE)/capi/capi_uart.h		\
	$(INCLUDE)/capi/capi_spi.h		\
	$(INCLUDE)/capi/capi_i2c.h		\
	$(INCLUDE)/capi/capi_dma.h		\
	$(INCLUDE)/capi/capi_gpio.h		\
	$(INCLUDE)/capi/capi_timer.h		\
	$(INCLUDE)/capi/capi_irq.h		\
	$(PLATFORM_DRIVERS)/stm32_capi_uart.h	\
	$(PLATFORM_DRIVERS)/stm32_capi_spi.h	\
	$(PLATFORM_DRIVERS)/stm32_capi_i2c.h	\
	$(PLATFORM_DRIVERS)/stm32_capi_dma.h	\
	$(PLATFORM_DRIVERS)/stm32_capi_gpio.h	\
	$(PLATFORM_DRIVERS)/stm32_capi_timer.h	\
	$(PLATFORM_DRIVERS)/stm32_capi_irq.h

# CAPI sources
SRCS += $(DRIVERS)/api/capi/capi_uart.c		\
	$(DRIVERS)/api/capi/capi_spi.c		\
	$(DRIVERS)/api/capi/capi_i2c.c		\
	$(DRIVERS)/api/capi/capi_dma.c		\
	$(DRIVERS)/api/capi/capi_gpio.c		\
	$(DRIVERS)/api/capi/capi_timer.c	\
	$(PLATFORM_DRIVERS)/stm32_capi_uart.c	\
	$(PLATFORM_DRIVERS)/stm32_capi_spi.c	\
	$(PLATFORM_DRIVERS)/stm32_capi_i2c.c	\
	$(PLATFORM_DRIVERS)/stm32_capi_dma.c	\
	$(PLATFORM_DRIVERS)/stm32_capi_gpio.c	\
	$(PLATFORM_DRIVERS)/stm32_capi_timer.c	\
	$(PLATFORM_DRIVERS)/stm32_capi_irq.c
