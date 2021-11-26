# See No-OS/tool/scripts/src_model.mk for variable description
SRC_DIRS += $(PROJECT)/src/app
SRC_DIRS += $(NO-OS)/iio/iio_app

SRCS +=	$(NO-OS)/util/list.c \
	$(NO-OS)/util/fifo.c \
	$(NO-OS)/util/util.c

#drivers
SRCS += $(DRIVERS)/adc/adc_demo/adc_demo.c \
	$(DRIVERS)/adc/adc_demo/iio_adc_demo.c \
	$(DRIVERS)/dac/dac_demo/iio_dac_demo.c \
	$(DRIVERS)/dac/dac_demo/dac_demo.c

INCS += $(INCLUDE)/fifo.h \
	$(INCLUDE)/uart.h \
	$(INCLUDE)/list.h \
	$(INCLUDE)/util.h \
	$(INCLUDE)/error.h

INCS += $(DRIVERS)/adc/adc_demo/iio_adc_demo.h \
		$(DRIVERS)/dac/dac_demo/dac_demo.h \
		$(DRIVERS)/dac/dac_demo/iio_dac_demo.h \
		$(DRIVERS)/adc/adc_demo/adc_demo.h

ifeq ($(PLATFORM),$(filter $(PLATFORM),xilinx aducm3029))
SRCS += $(PLATFORM_DRIVERS)/delay.c \
	$(DRIVERS)/api/irq.c
endif
INCS += $(INCLUDE)/delay.h

ifeq ($(PLATFORM),$(filter $(PLATFORM),xilinx aducm3029))
# For the moment there is support only for aducm for iio with network backend
ifeq (aducm3029,$(strip $(PLATFORM)))
ifeq '$(USE_TCP_SOCKET)' 'y'
CFLAGS += -DUSE_TCP_SOCKET
endif
ENABLE_IIO_NETWORK = y
endif

ifeq (y,$(strip $(ENABLE_IIO_NETWORK)))
DISABLE_SECURE_SOCKET ?= y
SRC_DIRS += $(NO-OS)/network
SRCS	 += $(NO-OS)/util/circular_buffer.c
SRCS	 += $(PLATFORM_DRIVERS)/timer.c
INCS	 += $(INCLUDE)/timer.h \
		$(INCLUDE)/circular_buffer.h \
		$(PLATFORM_DRIVERS)/timer_extra.h \
		$(PLATFORM_DRIVERS)/rtc_extra.h
endif

SRCS += $(PLATFORM_DRIVERS)/uart.c \
		$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c

INCS += $(INCLUDE)/irq.h \
	$(INCLUDE)/rtc.h \
	$(INCLUDE)/gpio.h \
	$(PLATFORM_DRIVERS)/irq_extra.h \
	$(PLATFORM_DRIVERS)/uart_extra.h
endif

# stm32
ifeq (stm32, $(PLATFORM))
SRCS += $(PLATFORM_DRIVERS)/stm32_delay.c \
	$(PLATFORM_DRIVERS)/stm32_uart.c \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.c
INCS += $(PLATFORM_DRIVERS)/stm32_uart_stdio.h \
	$(PLATFORM_DRIVERS)/stm32_uart.h \
	$(PLATFORM_DRIVERS)/stm32_hal.h
# SRCS += $(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
# 	$(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
# 	$(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
# 	$(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c \
# 	$(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
# 
# SRCS += $(PLATFORM_DRIVERS)/stm32_delay.c \
# 	$(PLATFORM_DRIVERS)/stm32_uart.c \
# 	$(PLATFORM_DRIVERS)/stm32_uart_stdio.c \
# 	$(PROJECT)/src/hal/stm32/stm32f4xx_hal_msp.c \
# 	$(PROJECT)/src/hal/stm32/stm32f4xx_it.c \
# 	$(PROJECT)/src/hal/stm32/sysmem.c \
# 	$(PROJECT)/src/hal/stm32/system_stm32f4xx.c
# 
# ASM_SRCS = $(PROJECT)/src/startup/startup_stm32f446retx.s
# 
# INCS += $(PROJECT)/src/hal/stm32/main.h \
# 	$(INCLUDE)/delay.h \
# 	$(PLATFORM_DRIVERS)/stm32_uart_stdio.h \
# 	$(PROJECT)/src/hal/stm32/stm32f4xx_hal_conf.h \
# 	$(PROJECT)/src/hal/stm32/stm32f4xx_it.h \
# 	$(PROJECT)/src/hal/stm32/stm32_hal.h \
# 	$(PLATFORM_DRIVERS)/stm32_uart.h
# 
# LSCRIPT = $(PROJECT)/src/linker/STM32F446RETX_FLASH.ld
endif

ifeq (linux,$(PLATFORM))
CFLAGS += -DENABLE_IIO_NETWORK \
			-DDISABLE_SECURE_SOCKET

LIBRARIES += iio
SRCS += $(NO-OS)/network/linux_socket/linux_socket.c 
SRCS +=	$(NO-OS)/network/tcp_socket.c
SRCS += $(PROJECT)/src/app/main.c
SRCS += $(NO-OS)/iio/iio_app/iio_app.c
SRCS += $(NO-OS)/util/circular_buffer.c

SRCS += $(DRIVERS)/platform/generic/uart.c \
		$(DRIVERS)/platform/generic/delay.c

INCS += $(NO-OS)/network/tcp_socket.h \
		$(NO-OS)/network/network_interface.h \
		$(NO-OS)/network/noos_mbedtls_config.h \
		$(NO-OS)/network/linux_socket/linux_socket.h

INCS	 += $(INCLUDE)/circular_buffer.h
INCS += $(PROJECT)/src/app/app_config.h \
		$(PROJECT)/src/app/parameters.h	

INCS += $(NO-OS)/iio/iio_app/iio_app.h 

INCS += $(INCLUDE)/gpio.h \
		$(INCLUDE)/delay.h \
		$(INCLUDE)/irq.h \
		$(INCLUDE)/trng.h

endif
