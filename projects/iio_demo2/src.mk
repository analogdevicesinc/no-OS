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

INCS += $(INCLUDE)/no-os/fifo.h \
	$(INCLUDE)/no-os/uart.h \
	$(INCLUDE)/no-os/list.h \
	$(INCLUDE)/no-os/util.h \
	$(INCLUDE)/no-os/error.h

INCS += $(DRIVERS)/adc/adc_demo/iio_adc_demo.h \
		$(DRIVERS)/dac/dac_demo/dac_demo.h \
		$(DRIVERS)/dac/dac_demo/iio_dac_demo.h \
		$(DRIVERS)/adc/adc_demo/adc_demo.h \

ifeq ($(PLATFORM),$(filter $(PLATFORM),xilinx aducm3029))
SRCS += $(PLATFORM_DRIVERS)/delay.c \
	$(DRIVERS)/irq/irq.c
endif
INCS += $(INCLUDE)/no-os/delay.h

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
INCS	 += $(INCLUDE)/no-os/timer.h \
		$(INCLUDE)/no-os/circular_buffer.h \
		$(PLATFORM_DRIVERS)/timer_extra.h \
		$(PLATFORM_DRIVERS)/rtc_extra.h
endif

SRCS += $(PLATFORM_DRIVERS)/uart.c \
		$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c

INCS += $(INCLUDE)/no-os/irq.h \
	$(INCLUDE)/no-os/rtc.h \
	$(INCLUDE)/no-os/gpio.h \
	$(PLATFORM_DRIVERS)/irq_extra.h \
	$(PLATFORM_DRIVERS)/uart_extra.h
endif

# stm32
ifeq (stm32, $(PLATFORM))
SRCS += $(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
	$(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
	$(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
	$(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c \
	$(STM32CUBE)/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c

SRCS += $(PLATFORM_DRIVERS)/stm32_delay.c \
	$(PLATFORM_DRIVERS)/stm32_uart.c \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.c \
	$(PROJECT)/src/hal/stm32/stm32f4xx_hal_msp.c \
	$(PROJECT)/src/hal/stm32/stm32f4xx_it.c \
	$(PROJECT)/src/hal/stm32/sysmem.c \
	$(PROJECT)/src/hal/stm32/system_stm32f4xx.c

ASM_SRCS = $(PROJECT)/src/startup/startup_stm32f446retx.s

INCS += $(PROJECT)/src/hal/stm32/main.h \
	$(INCLUDE)/no-os/delay.h \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.h \
	$(PROJECT)/src/hal/stm32/stm32f4xx_hal_conf.h \
	$(PROJECT)/src/hal/stm32/stm32f4xx_it.h \
	$(PROJECT)/src/hal/stm32/stm32_hal.h \
	$(PLATFORM_DRIVERS)/stm32_uart.h

LSCRIPT = $(PROJECT)/src/linker/STM32F446RETX_FLASH.ld
endif


#maxim

ifeq (maxim, $(PLATFORM))

SRCS += $(PLATFORM_DRIVERS)/uart_maxim.c 	\
	$(PLATFORM_DRIVERS)/stdio.c			\
	$(MAXIM_LIBRARIES)/MAX32660PeriphDriver/Source/uart.c 	\
	$(PLATFORM_DRIVERS)/delay_maxim.c 	\
	$(MAXIM_LIBRARIES)/CMSIS/Device/Maxim/MAX32660/Source/heap.c	 \
	$(MAXIM_LIBRARIES)/CMSIS/Device/Maxim/MAX32660/Source/system_max32660.c	 \
	$(MAXIM_LIBRARIES)/MAX32660PeriphDriver/Source/mxc_delay.c			\
	$(MAXIM_LIBRARIES)/MAX32660PeriphDriver/Source/mxc_pins.c			\
	$(MAXIM_LIBRARIES)/MAX32660PeriphDriver/Source/mxc_lock.c			\
	$(MAXIM_LIBRARIES)/MAX32660PeriphDriver/Source/gpio.c			\
	$(MAXIM_LIBRARIES)/MAX32660PeriphDriver/Source/mxc_sys.c	 	

INCS += $(PLATFORM_DRIVERS)/uart_maxim.h			\
	$(MAXIM_LIBRARIES)/MAX32660PeriphDriver/Include/uart_maxim_pdriver.h			\
	$(MAXIM_LIBRARIES)/CMSIS/Device/Maxim/MAX32660/Include/max32660.h	 \
	$(MAXIM_LIBRARIES)/CMSIS/Device/Maxim/MAX32660/Include/mxc_device.h	 \
	$(MAXIM_LIBRARIES)/CMSIS/Device/Maxim/MAX32660/Include/system_max32660.h	 #\
#	$(MAXIM_LIBRARIES)/MAX32660PeriphDriver/Include/mxc_config.h			\
#	$(MAXIM_LIBRARIES)/MAX32660PeriphDriver/Include/mxc_errors.h			\
#	$(MAXIM_LIBRARIES)/MAX32660PeriphDriver/Include/mxc_assert.h			\
#	$(MAXIM_LIBRARIES)/MAX32660PeriphDriver/Include/mxc_lock.h			\
#	$(MAXIM_LIBRARIES)/MAX32660PeriphDriver/Include/mxc_pins.h			\
#	$(MAXIM_LIBRARIES)/MAX32660PeriphDriver/Include/rtc.h			\
#	$(MAXIM_LIBRARIES)/MAX32660PeriphDriver/Include/mxc_delay.h			\
#	$(MAXIM_LIBRARIES)/MAX32660PeriphDriver/Include/gpio.h			\
#	$(MAXIM_LIBRARIES)/MAX32660PeriphDriver/Include/dma.h			\
#	$(MAXIM_LIBRARIES)/MAX32660PeriphDriver/Include/mxc_sys.h	

ASM_SRCS = $(MAXIM_LIBRARIES)/CMSIS/Device/Maxim/MAX32660/Source/GCC/startup_max32660.S

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

INCS	 += $(INCLUDE)/no-os/circular_buffer.h
INCS += $(PROJECT)/src/app/app_config.h \
		$(PROJECT)/src/app/parameters.h	

INCS += $(NO-OS)/iio/iio_app/iio_app.h 

INCS += $(INCLUDE)/no-os/gpio.h \
		$(INCLUDE)/no-os/delay.h \
		$(INCLUDE)/no-os/irq.h \
		$(INCLUDE)/no-os/trng.h

endif
