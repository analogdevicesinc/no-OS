# See No-OS/tool/scripts/src_model.mk for variable description
SRCS += $(PROJECT)/src/app/main.c \
        $(DRIVERS)/display/ssd_1306/ssd_1306.c \
        $(DRIVERS)/display/display.c \
	$(DRIVERS)/api/no_os_gpio.c \
        $(DRIVERS)/api/no_os_spi.c \
        $(DRIVERS)/platform/xilinx/xilinx_spi.c \
        $(DRIVERS)/platform/xilinx/xilinx_gpio.c \
	$(NO-OS)/util/no_os_font_8x8.c

INCS += $(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_spi.h \
        $(DRIVERS)/display/ssd_1306/ssd_1306.h \
        $(DRIVERS)/display/display.h \
        $(PROJECT)/src/app/parameters.h \
        $(INCLUDE)/no_os_error.h \
        $(INCLUDE)/no_os_delay.h \
        $(DRIVERS)/platform/xilinx/gpio_extra.h \
	$(DRIVERS)/platform/xilinx/spi_extra.h
