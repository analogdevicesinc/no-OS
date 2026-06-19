INCS += $(DRIVERS)/net/adin1320/adin1320.h		\
	$(DRIVERS)/net/mdio_spi/mdio_spi.h		\
	$(INCLUDE)/no_os_alloc.h			\
	$(INCLUDE)/no_os_crc8.h				\
	$(INCLUDE)/no_os_crc16.h			\
	$(INCLUDE)/no_os_delay.h			\
	$(INCLUDE)/no_os_dma.h				\
	$(INCLUDE)/no_os_error.h			\
	$(INCLUDE)/no_os_gpio.h				\
	$(INCLUDE)/no_os_init.h				\
	$(INCLUDE)/no_os_irq.h				\
	$(INCLUDE)/no_os_lf256fifo.h			\
	$(INCLUDE)/no_os_list.h				\
	$(INCLUDE)/no_os_mdio.h				\
	$(INCLUDE)/no_os_mutex.h			\
	$(INCLUDE)/no_os_print_log.h			\
	$(INCLUDE)/no_os_pwm.h				\
	$(INCLUDE)/no_os_spi.h				\
	$(INCLUDE)/no_os_timer.h			\
	$(INCLUDE)/no_os_uart.h				\
	$(INCLUDE)/no_os_units.h			\
	$(INCLUDE)/no_os_util.h

SRCS += $(DRIVERS)/net/adin1320/adin1320.c		\
	$(DRIVERS)/net/mdio_spi/mdio_spi.c		\
	$(DRIVERS)/api/no_os_dma.c			\
	$(DRIVERS)/api/no_os_gpio.c			\
	$(DRIVERS)/api/no_os_irq.c			\
	$(DRIVERS)/api/no_os_mdio.c			\
	$(DRIVERS)/api/no_os_pwm.c			\
	$(DRIVERS)/api/no_os_spi.c			\
	$(DRIVERS)/api/no_os_timer.c			\
	$(DRIVERS)/api/no_os_uart.c			\
	$(NO-OS)/util/no_os_alloc.c			\
	$(NO-OS)/util/no_os_crc8.c			\
	$(NO-OS)/util/no_os_crc16.c			\
	$(NO-OS)/util/no_os_lf256fifo.c			\
	$(NO-OS)/util/no_os_list.c			\
	$(NO-OS)/util/no_os_mutex.c			\
	$(NO-OS)/util/no_os_util.c
