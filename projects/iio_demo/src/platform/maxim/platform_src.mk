SRCS += $(PLATFORM_DRIVERS)/maxim_delay.c	\
	$(PLATFORM_DRIVERS)/maxim_irq.c		\
	$(PLATFORM_DRIVERS)/maxim_timer.c	\
	$(PLATFORM_DRIVERS)/maxim_uart.c	\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.c
	
INCS += $(PLATFORM_DRIVERS)/maxim_irq.h		\
	$(PLATFORM_DRIVERS)/maxim_uart.h	\
	$(PLATFORM_DRIVERS)/maxim_timer.h	\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.h

INCS += $(INCLUDE)/no_os_irq.h

SRCS += $(DRIVERS)/api/no_os_irq.c
SRCS += $(DRIVERS)/api/no_os_timer.c

SRCS += $(NO-OS)/util/no_os_lf256fifo.c

SRCS +=	$(PLATFORM_DRIVERS)/maxim_usb_uart.c
INCS += $(PLATFORM_DRIVERS)/maxim_usb_uart.h \
	$(PLATFORM_DRIVERS)/maxim_usb_uart_descriptors.h \

SRC_DIRS += $(MAXIM_LIBRARIES)/MAXUSB/src/core/musbhsfc
SRC_DIRS += $(MAXIM_LIBRARIES)/MAXUSB/src/enumerate
SRC_DIRS += $(MAXIM_LIBRARIES)/MAXUSB/src/devclass
SRC_DIRS += $(MAXIM_LIBRARIES)/MAXUSB/include/core/musbhsfc
SRC_DIRS += $(MAXIM_LIBRARIES)/MAXUSB/include/enumerate
SRC_DIRS += $(MAXIM_LIBRARIES)/MAXUSB/include/devclass

INCS += $(MAXIM_LIBRARIES)/MAXUSB/include/core/usb.h
INCS += $(MAXIM_LIBRARIES)/MAXUSB/include/core/usb_protocol.h
INCS += $(MAXIM_LIBRARIES)/MAXUSB/include/core/usb_event.h

SRCS += $(MAXIM_LIBRARIES)/MAXUSB/src/core/usb_event.c
