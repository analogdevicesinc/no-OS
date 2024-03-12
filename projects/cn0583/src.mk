SRC_DIRS += $(PROJECT)/srcs

SRC_DIRS += $(PLATFORM_DRIVERS)
SRC_DIRS += $(INCLUDE)
SRC_DIRS += $(NO-OS)/util
SRC_DIRS += $(DRIVERS)/api
SRC_DIRS += $(DRIVERS)/sd-card

INCS += $(NO-OS)/libraries/fatfs/source/ff.h
INCS += $(NO-OS)/libraries/fatfs/source/ffconf.h
INCS += $(NO-OS)/libraries/fatfs/source/diskio.h

SRCS += $(NO-OS)/libraries/fatfs/source/ff.c
SRCS += $(NO-OS)/libraries/fatfs/source/ffsystem.c
SRCS += $(NO-OS)/libraries/fatfs/source/ffunicode.c
SRCS += $(NO-OS)/libraries/fatfs/adi_diskio.c

SRCS += $(NO-OS)/util/no_os_list.c
SRCS += $(NO-OS)/util/no_os_util.c
SRCS += $(DRIVERS)/temperature/max31875/max31875.c
SRCS += $(DRIVERS)/photo-electronic/adpd188/adpd188.c

INCS += $(DRIVERS)/photo-electronic/adpd188/adpd188.h
INCS += $(DRIVERS)/temperature/max31875/max31875.h

SRCS += $(PLATFORM_DRIVERS)/maxim_uart.c 			\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.c			\
	$(PLATFORM_DRIVERS)/maxim_gpio.c			\
	$(PLATFORM_DRIVERS)/maxim_irq.c				\
	$(PLATFORM_DRIVERS)/maxim_gpio_irq.c			\
	$(PLATFORM_DRIVERS)/maxim_rtc.c				\
	$(PLATFORM_DRIVERS)/maxim_spi.c				\
	$(PLATFORM_DRIVERS)/maxim_delay.c 			\
	$(NO-OS)/util/no_os_lf256fifo.c

INCS += $(PLATFORM_DRIVERS)/maxim_uart.h			\
	$(PLATFORM_DRIVERS)/maxim_gpio.h			\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.h			\
	$(PLATFORM_DRIVERS)/maxim_irq.h				\
	$(PLATFORM_DRIVERS)/maxim_init.h			\
	$(PLATFORM_DRIVERS)/maxim_gpio_irq.h			\
	$(PLATFORM_DRIVERS)/maxim_spi.h				\
	$(PLATFORM_DRIVERS)/maxim_rtc.h
