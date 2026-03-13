INCS += $(INCLUDE)/no_os_delay.h		\
	$(INCLUDE)/no_os_dma.h			\
	$(INCLUDE)/no_os_error.h		\
	$(INCLUDE)/no_os_eeprom.h		\
	$(INCLUDE)/no_os_gpio.h			\
	$(INCLUDE)/no_os_print_log.h		\
	$(INCLUDE)/no_os_i2c.h			\
	$(INCLUDE)/no_os_spi.h			\
	$(INCLUDE)/no_os_irq.h			\
	$(INCLUDE)/no_os_list.h			\
	$(INCLUDE)/no_os_uart.h			\
	$(INCLUDE)/no_os_lf256fifo.h		\
	$(INCLUDE)/no_os_util.h			\
	$(INCLUDE)/no_os_units.h		\
	$(INCLUDE)/no_os_alloc.h		\
	$(INCLUDE)/no_os_mutex.h

SRCS += $(DRIVERS)/api/no_os_dma.c		\
	$(DRIVERS)/api/no_os_gpio.c		\
	$(NO-OS)/util/no_os_lf256fifo.c		\
	$(DRIVERS)/api/no_os_eeprom.c		\
	$(DRIVERS)/api/no_os_irq.c		\
	$(DRIVERS)/api/no_os_i2c.c		\
	$(DRIVERS)/api/no_os_spi.c		\
	$(DRIVERS)/api/no_os_uart.c		\
	$(NO-OS)/util/no_os_list.c		\
	$(NO-OS)/util/no_os_util.c		\
	$(NO-OS)/util/no_os_alloc.c		\
	$(NO-OS)/util/no_os_mutex.c

INCS += $(DRIVERS)/position/admt4000/admt4000.h
SRCS += $(DRIVERS)/position/admt4000/admt4000.c

INCS += $(DRIVERS)/eeprom/24xx32a/24xx32a.h
SRCS += $(DRIVERS)/eeprom/24xx32a/24xx32a.c

INCS += $(DRIVERS)/motor/tmc5240/tmc5240.h
SRCS += $(DRIVERS)/motor/tmc5240/tmc5240.c

INCS += $(NO-OS)/libraries/precision-converters-library/common/common.h
INCS += $(NO-OS)/libraries/precision-converters-library/board_info/board_info.h
SRCS += $(NO-OS)/libraries/precision-converters-library/common/common.c
SRCS += $(NO-OS)/libraries/precision-converters-library/board_info/board_info.c

ifeq (y,$(strip $(TMC_ENABLED)))
CFLAGS += -DTMC=1
LIBRARIES += tmc_api_TMC5240
CFLAGS += -I$(NO-OS)/libraries/TMC/TMC-API
endif

CFLAGS += -DIIO_IGNORE_BUFF_OVERRUN_ERR
