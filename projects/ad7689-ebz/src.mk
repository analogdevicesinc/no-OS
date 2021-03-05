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

# common
SRCS +=	$(PROJECT)/src/app/headless.c \
	$(DRIVERS)/spi/spi.c \
	$(NO-OS)/drivers/gpio/gpio.c \
	$(NO-OS)/util/util.c \
	$(NO-OS)/drivers/adc/ad7689/ad7689.c

INCS +=	$(INCLUDE)/spi.h \
	$(PLATFORM_DRIVERS)/spi_extra.h \
	$(INCLUDE)/gpio.h \
	$(PLATFORM_DRIVERS)/gpio_extra.h \
	$(INCLUDE)/error.h \
	$(INCLUDE)/print_log.h \
	$(INCLUDE)/delay.h \
	$(INCLUDE)/util.h \
	$(INCLUDE)/uart.h \
	$(NO-OS)/drivers/adc/ad7689/ad7689.h

# stm32
ifeq (stm32, $(PLATFORM))
SRCS += $(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
	$(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
	$(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c \
	$(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
	$(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
	$(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
	$(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
	$(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
	$(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c

SRCS += $(PLATFORM_DRIVERS)/stm32_gpio.c \
	$(PLATFORM_DRIVERS)/stm32_delay.c \
	$(PLATFORM_DRIVERS)/stm32_spi.c \
	$(PLATFORM_DRIVERS)/stm32_uart.c \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.c \
	$(PROJECT)/src/hal/stm32/stm32f4xx_hal_msp.c \
	$(PROJECT)/src/hal/stm32/stm32f4xx_it.c \
	$(PROJECT)/src/hal/stm32/sysmem.c \
	$(PROJECT)/src/hal/stm32/system_stm32f4xx.c

INCS += $(PLATFORM_DRIVERS)/stm32_gpio.h \
	$(PLATFORM_DRIVERS)/stm32_spi.h \
	$(PLATFORM_DRIVERS)/stm32_uart.h \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.h

ASM_SRCS = $(PROJECT)/src/startup/startup_stm32f446retx.s

INCS += $(PROJECT)/src/hal/stm32/main.h \
	$(PROJECT)/src/hal/stm32/stm32f4xx_hal_conf.h \
	$(PROJECT)/src/hal/stm32/stm32f4xx_it.h \
	$(PROJECT)/src/hal/stm32/stm32_hal.h \
	$(PROJECT)/src/hal/stm32/parameters.h

LSCRIPT = $(PROJECT)/src/linker/STM32F446RETX_FLASH.ld
endif
