include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk
include $(PROJECT)/src/examples/examples_src.mk

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c

INCS += $(PROJECT)/src/common/common_data.h
SRCS += $(PROJECT)/src/common/common_data.c

INCS += $(PROJECT)/src/platform/platform_includes.h

INCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.h
SRCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.c 

SRCS += $(DRIVERS)/api/no_os_uart.c     \
    $(NO-OS)/util/no_os_fifo.c      \
    $(NO-OS)/util/no_os_list.c      \
    $(NO-OS)/util/no_os_util.c      \
    $(NO-OS)/util/no_os_font_8x8.c  \
    $(NO-OS)/util/no_os_alloc.c     

INCS += $(INCLUDE)/no_os_delay.h     \
    $(INCLUDE)/no_os_error.h     \
    $(INCLUDE)/no_os_fifo.h      \
    $(INCLUDE)/no_os_irq.h       \
    $(INCLUDE)/no_os_lf256fifo.h \
    $(INCLUDE)/no_os_list.h      \
    $(INCLUDE)/no_os_timer.h     \
    $(INCLUDE)/no_os_uart.h      \
    $(INCLUDE)/no_os_gpio.h      \
    $(INCLUDE)/no_os_util.h      \
    $(INCLUDE)/no_os_alloc.h     \
    $(INCLUDE)/no_os_mutex.h     \
    $(INCLUDE)/no_os_semaphore.h

# Platform specific, free-rtos config file
FREERTOS_CONFIG_PATH := $(PROJECT)/src/platform/$(PLATFORM)/FreeRTOSConfig.h

# Project and platform specific lvgl library config file
LVGL_CONFIG_PATH := $(PROJECT)/lv_conf.h
