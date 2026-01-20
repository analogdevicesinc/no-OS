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

# Trinamic motor driver support
# Include TMC driver if TMC_DEVICE is defined and not empty
ifneq ($(strip $(TMC_DEVICE)),)
# Convert device name to lowercase for directory/file paths
TMC_DEVICE_LOWER = $(shell echo $(TMC_DEVICE) | tr A-Z a-z)

# Add TMC-specific flags and includes
CFLAGS += -DTMC=1 -DTMC_DEVICE=$(TMC_DEVICE)
CFLAGS += -I$(NO-OS)/libraries/tmc/TMC-API
LIBRARIES += tmc_api_$(TMC_DEVICE)

# Include device-specific driver
INCS += $(DRIVERS)/motor/$(TMC_DEVICE_LOWER)/$(TMC_DEVICE_LOWER).h
SRCS += $(DRIVERS)/motor/$(TMC_DEVICE_LOWER)/$(TMC_DEVICE_LOWER).c
endif
