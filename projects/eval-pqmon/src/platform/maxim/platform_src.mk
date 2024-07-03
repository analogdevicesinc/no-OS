INCS += $(PLATFORM_DRIVERS)/maxim_delay.h     		\
        $(PLATFORM_DRIVERS)/maxim_gpio.h      		\
        $(PLATFORM_DRIVERS)/maxim_spi.h       		\
        $(PLATFORM_DRIVERS)/maxim_init.h      		\
        $(PLATFORM_DRIVERS)/maxim_gpio_irq.h  		\
        $(PLATFORM_DRIVERS)/maxim_irq.h       		\
        $(PLATFORM_DRIVERS)/maxim_i2c.h       		\
        $(PLATFORM_DRIVERS)/maxim_timer.h     		\
        $(PLATFORM_DRIVERS)/maxim_uart.h      		\
        $(PLATFORM_DRIVERS)/../common/maxim_dma.h	\
        $(PLATFORM_DRIVERS)/maxim_uart_stdio.h 		\
	$(PLATFORM_DRIVERS)/maxim_usb_uart.h 		\
	$(PLATFORM_DRIVERS)/maxim_usb_uart_descriptors.h 

SRCS += $(PLATFORM_DRIVERS)/maxim_delay.c     		\
        $(PLATFORM_DRIVERS)/maxim_gpio.c      		\
        $(PLATFORM_DRIVERS)/maxim_spi.c       		\
        $(PLATFORM_DRIVERS)/maxim_init.c      		\
        $(PLATFORM_DRIVERS)/maxim_i2c.c       		\
        $(PLATFORM_DRIVERS)/maxim_timer.c     		\
        $(PLATFORM_DRIVERS)/maxim_gpio_irq.c  		\
        $(PLATFORM_DRIVERS)/maxim_irq.c       		\
        $(PLATFORM_DRIVERS)/maxim_uart.c      		\
        $(PLATFORM_DRIVERS)/../common/maxim_dma.c      	\
        $(PLATFORM_DRIVERS)/maxim_uart_stdio.c 		\
	$(PLATFORM_DRIVERS)/maxim_usb_uart.c

SRC_DIRS += 	$(MAXIM_LIBRARIES)/MAXUSB/src/core/musbhsfc	\
		$(MAXIM_LIBRARIES)/MAXUSB/src/enumerate		\
		$(MAXIM_LIBRARIES)/MAXUSB/src/devclass		\
		$(MAXIM_LIBRARIES)/MAXUSB/include/core/musbhsfc	\
		$(MAXIM_LIBRARIES)/MAXUSB/include/enumerate	\
		$(MAXIM_LIBRARIES)/MAXUSB/include/devclass

INCS +=	$(MAXIM_LIBRARIES)/MAXUSB/include/core/usb.h		\
	$(MAXIM_LIBRARIES)/MAXUSB/include/core/usb_protocol.h	\
	$(MAXIM_LIBRARIES)/MAXUSB/include/core/usb_event.h	
	
SRCS +=	$(MAXIM_LIBRARIES)/MAXUSB/src/core/usb_event.c

EXTRA_MATH_LIB = $(MAXIM_LIBRARIES)/CMSIS/5.9.0/DSP/Lib/libarm_cortexM4l_math.a
