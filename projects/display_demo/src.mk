# See No-OS/tool/scripts/src_model.mk for variable description
SRCS += $(PROJECT)/src/app/main.c \
        $(DRIVERS)/display/ssd_1306/ssd_1306.c \
        $(DRIVERS)/display/display.c \
	$(DRIVERS)/api/no_os_gpio.c \
        $(DRIVERS)/api/no_os_spi.c \
        $(DRIVERS)/platform/xilinx/xilinx_spi.c \
        $(DRIVERS)/platform/xilinx/xilinx_gpio.c \
	$(NO-OS)/util/no_os_font_8x8.c \
        $(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c

INCS += $(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_spi.h \
        $(DRIVERS)/display/ssd_1306/ssd_1306.h \
        $(DRIVERS)/display/display.h \
        $(PROJECT)/src/app/parameters.h \
        $(INCLUDE)/no_os_error.h \
        $(INCLUDE)/no_os_delay.h \
        $(INCLUDE)/no_os_alloc.h \
        $(INCLUDE)/no_os_mutex.h \
        $(DRIVERS)/platform/xilinx/$(PLATFORM)_gpio.h \
	$(DRIVERS)/platform/xilinx/$(PLATFORM)_spi.h
