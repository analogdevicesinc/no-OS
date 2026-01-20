ifeq (y,$(strip $(SIMULATION)))
CFLAGS += -DNO_OS_LATTICE_SIMULATION
endif

SRCS += $(PLATFORM_DRIVERS)/riscvrx_init.c \
	$(PLATFORM_DRIVERS)/riscvrx_delay.c \
	$(PLATFORM_DRIVERS)/riscvrx_gpio.c \
	$(PLATFORM_DRIVERS)/riscvrx_irq.c \
	$(PLATFORM_DRIVERS)/riscvrx_timer.c \
	$(PLATFORM_DRIVERS)/riscvrx_uart.c \
	$(PLATFORM_DRIVERS)/riscvrx_uart_stdio.c \
	$(PLATFORM_DRIVERS)/riscvrx_ip_uart.c \
	$(PLATFORM_DRIVERS)/riscvrx_axi_io.c \
	$(PLATFORM_DRIVERS)/riscvrx_cache.c

INCS += $(PLATFORM_DRIVERS)/riscvrx_gpio.h \
	$(PLATFORM_DRIVERS)/riscvrx_irq.h \
	$(PLATFORM_DRIVERS)/riscvrx_timer.h \
	$(PLATFORM_DRIVERS)/riscvrx_uart.h \
	$(PLATFORM_DRIVERS)/riscvrx_ip_uart.h \
	$(PLATFORM_DRIVERS)/riscvrx_cache.h
