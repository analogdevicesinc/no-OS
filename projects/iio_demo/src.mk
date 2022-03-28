# See No-OS/tool/scripts/src_model.mk for variable description
SRC_DIRS += $(PROJECT)/src/app
SRC_DIRS += $(NO-OS)/iio/iio_app

SRCS +=	$(NO-OS)/util/no_os_list.c \
	$(NO-OS)/util/no_os_fifo.c \
	$(NO-OS)/util/no_os_util.c

#drivers
SRCS += $(DRIVERS)/adc/adc_demo/adc_demo.c \
	$(DRIVERS)/adc/adc_demo/iio_adc_demo.c \
	$(DRIVERS)/dac/dac_demo/iio_dac_demo.c \
	$(DRIVERS)/dac/dac_demo/dac_demo.c

INCS += $(INCLUDE)/no_os_fifo.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_list.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_error.h

INCS += $(DRIVERS)/adc/adc_demo/iio_adc_demo.h \
		$(DRIVERS)/dac/dac_demo/dac_demo.h \
		$(DRIVERS)/dac/dac_demo/iio_dac_demo.h \
		$(DRIVERS)/adc/adc_demo/adc_demo.h

ifeq ($(PLATFORM),$(filter $(PLATFORM),xilinx aducm3029))
SRCS += $(PLATFORM_DRIVERS)/delay.c \
	$(DRIVERS)/api/no_os_irq.c
endif
INCS += $(INCLUDE)/no_os_delay.h

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
SRCS	 += $(NO-OS)/util/no_os_circular_buffer.c
SRCS	 += $(PLATFORM_DRIVERS)/no_os_timer.c
INCS	 += $(INCLUDE)/no_os_timer.h \
		$(INCLUDE)/no_os_circular_buffer.h \
		$(PLATFORM_DRIVERS)/timer_extra.h \
		$(PLATFORM_DRIVERS)/rtc_extra.h
endif

SRCS += $(PLATFORM_DRIVERS)/no_os_uart.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
		$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c

INCS += $(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_rtc.h \
	$(INCLUDE)/no_os_gpio.h \
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
endif

ifeq (linux,$(PLATFORM))
CFLAGS += -DENABLE_IIO_NETWORK \
			-DDISABLE_SECURE_SOCKET

LIBRARIES += iio
SRCS += $(NO-OS)/network/linux_socket/linux_socket.c 
SRCS +=	$(NO-OS)/network/tcp_socket.c
SRCS += $(PROJECT)/src/app/main.c
SRCS += $(NO-OS)/iio/iio_app/iio_app.c
SRCS += $(NO-OS)/util/no_os_circular_buffer.c

SRCS += $(DRIVERS)/platform/generic/no_os_uart.c \
		$(DRIVERS)/platform/generic/delay.c

INCS += $(NO-OS)/network/tcp_socket.h \
		$(NO-OS)/network/network_interface.h \
		$(NO-OS)/network/noos_mbedtls_config.h \
		$(NO-OS)/network/linux_socket/linux_socket.h

INCS	 += $(INCLUDE)/no_os_circular_buffer.h
INCS += $(PROJECT)/src/app/app_config.h \
		$(PROJECT)/src/app/parameters.h	

INCS += $(NO-OS)/iio/iio_app/iio_app.h 

INCS += $(INCLUDE)/no_os_gpio.h \
		$(INCLUDE)/no_os_delay.h \
		$(INCLUDE)/no_os_irq.h \
		$(INCLUDE)/no_os_trng.h

endif
