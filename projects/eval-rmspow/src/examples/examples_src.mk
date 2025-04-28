
SRCS += $(PROJECT)/src/examples/rmspow_firmware/rmspow_firmware.c
INCS += $(PROJECT)/src/examples/rmspow_firmware/rmspow_firmware.h

INCS += $(INCLUDE)/no_os_list.h \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h

SRCS += $(NO-OS)/drivers/display/ssd_1306/ssd_1306.c 
SRCS += $(NO-OS)/drivers/display/display.c 
INCS += $(NO-OS)/drivers/display/ssd_1306/ssd_1306.h
INCS += $(NO-OS)/drivers/display/display.h