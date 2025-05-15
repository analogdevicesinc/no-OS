INCS += $(INCLUDE)/no_os_delay.h		\
	$(INCLUDE)/no_os_dma.h			\
	$(INCLUDE)/no_os_error.h		\
	$(INCLUDE)/no_os_gpio.h			\
	$(INCLUDE)/no_os_print_log.h		\
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
	$(DRIVERS)/api/no_os_irq.c		\
	$(DRIVERS)/api/no_os_spi.c		\
	$(DRIVERS)/api/no_os_uart.c		\
	$(NO-OS)/util/no_os_list.c		\
	$(NO-OS)/util/no_os_util.c		\
	$(NO-OS)/util/no_os_alloc.c		\
	$(NO-OS)/util/no_os_mutex.c

INCS += $(DRIVERS)/magnetometer/admt4000/admt4000.h
SRCS += $(DRIVERS)/magnetometer/admt4000/admt4000.c

ifeq (y,$(strip $(TMC_ENABLED)))
CFLAGS += -DTMC=1

TMC_SRC1 := $(wildcard $(addsuffix /*.c, $(NO-OS)/libraries/TMC-API/tmc/helpers))
TMC_INC1 := $(wildcard $(addsuffix /*.h, $(NO-OS)/libraries/TMC-API/tmc/helpers))
SRCS += $(TMC_SRC1)
INCS += $(TMC_INC1)

TMC_SRC2 := $(wildcard $(addsuffix /*.c, $(NO-OS)/libraries/TMC-API/tmc/ramp))
TMC_INC2 := $(wildcard $(addsuffix /*.h, $(NO-OS)/libraries/TMC-API/tmc/ramp))
SRCS += $(TMC_SR2)
INCS += $(TMC_INC2)

# Add TMC5240 related files
TMC_SRC3 := $(wildcard $(addsuffix /*.c, $(NO-OS)/libraries/TMC-API/tmc/ic/TMC5240))
TMC_INC3 := $(wildcard $(addsuffix /*.h, $(NO-OS)/libraries/TMC-API/tmc/ic/TMC5240))
SRCS += $(TMC_SRC3)
INCS += $(TMC_INC3)

endif
