
SRCS += $(PROJECT)/src/example.c

# Common no_os headers needed by CAPI drivers
INCS +=$(PLATFORM_DRIVERS)/stm32_hal.h	\
	$(INCLUDE)/no_os_error.h
# 	$(PLATFORM_DRIVERS)/stm32_pwm.h	\
# 	$(PLATFORM_DRIVERS)/stm32_gpio.h	\
# 	$(PLATFORM_DRIVERS)/stm32_irq.h

# Common no_os sources needed by CAPI drivers
#SRCS += $(PLATFORM_DRIVERS)/stm32_delay.c	
# 	$(DRIVERS)/api/no_os_pwm.c	\
# 	$(DRIVERS)/api/no_os_gpio.c	\
# 	$(DRIVERS)/api/no_os_irq.c	\
# 	$(PLATFORM_DRIVERS)/stm32_pwm.c	\
# 	$(PLATFORM_DRIVERS)/stm32_gpio.c	\
# 	$(PLATFORM_DRIVERS)/stm32_irq.c

# CAPI headers
INCS += $(NO-OS)/capi/inc/capi_uart.h		\
	$(NO-OS)/capi/inc/capi_alloc.h		\
	$(NO-OS)/capi/inc/capi_time.h		\
	$(PLATFORM_DRIVERS)/stm32_capi_uart.h	\
 	$(NO-OS)/capi/inc/capi_gpio.h		\
 	$(PLATFORM_DRIVERS)/stm32_capi_gpio.h	\
 	$(PLATFORM_DRIVERS)/stm32_capi_dma.h	\
 	$(NO-OS)/capi/inc/capi_dma.h		\
	$(NO-OS)/capi/inc/capi_spi.h		\
 	$(PLATFORM_DRIVERS)/stm32_capi_spi.h	\
 	$(NO-OS)/capi/inc/capi_i2c.h		\
 	$(PLATFORM_DRIVERS)/stm32_capi_i2c.h	\
 	$(NO-OS)/capi/inc/capi_irq.h		\
 	$(PLATFORM_DRIVERS)/stm32_capi_irq.h	\
 	$(NO-OS)/capi/inc/capi_timer.h		\
 	$(PLATFORM_DRIVERS)/stm32_capi_timer.h	\
	$(PLATFORM_DRIVERS)/stm32_capi_pwm.h

# CAPI sources
SRCS += $(NO-OS)/capi/src/capi_uart.c		\
	$(NO-OS)/capi/src/capi_alloc.c		\
	$(NO-OS)/capi/src/capi_time.c		\
	$(PLATFORM_DRIVERS)/stm32_capi_time.c	\
	$(PLATFORM_DRIVERS)/stm32_capi_uart.c	\
	$(PLATFORM_DRIVERS)/stm32_capi_alloc.c  \
 	$(NO-OS)/capi/src/capi_gpio.c		\
 	$(PLATFORM_DRIVERS)/stm32_capi_gpio.c	\
 	$(NO-OS)/capi/src/capi_dma.c		\
 	$(PLATFORM_DRIVERS)/stm32_capi_dma.c	\
 	$(NO-OS)/capi/src/capi_spi.c		\
 	$(PLATFORM_DRIVERS)/stm32_capi_spi.c	\
 	$(NO-OS)/capi/src/capi_i2c.c		\
	$(PLATFORM_DRIVERS)/stm32_capi_i2c.c	\
 	$(PLATFORM_DRIVERS)/stm32_capi_irq.c	\
 	$(NO-OS)/capi/src/capi_timer.c		\
 	$(PLATFORM_DRIVERS)/stm32_capi_timer.c	\
	$(PLATFORM_DRIVERS)/stm32_capi_pwm.c

	
# 	$(INCLUDE)/no_os_alloc.h	\
# 	$(INCLUDE)/no_os_util.h		\
# 	$(INCLUDE)/no_os_delay.h	\
# 	$(INCLUDE)/no_os_pwm.h		\
# 	$(INCLUDE)/no_os_gpio.h		\
# 	$(INCLUDE)/no_os_irq.h		\
# 	$(INCLUDE)/no_os_mutex.h	\
# 	$(INCLUDE)/no_os_lf256fifo.h	\
# 	$(INCLUDE)/no_os_list.h		\

# $(NO-OS)/util/no_os_alloc.c	\
# 	$(NO-OS)/util/no_os_mutex.c	\
# 	$(NO-OS)/util/no_os_lf256fifo.c	\
# 	$(NO-OS)/util/no_os_list.c	\