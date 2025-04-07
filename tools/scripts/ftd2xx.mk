ifeq ($(wildcard $(NO-OS)/libraries/ftd2xx/release),)
        SCRIPT_CFG := $(shell mkdir -p $(NO-OS)/libraries/ftd2xx)
        SCRIPT_CFG := $(shell python $(NO-OS)/tools/scripts/config_ftd2xx.py $(NO-OS)/libraries/ftd2xx)
endif

CFLAGS += -DFT_VER_MAJOR
CFLAGS += -DFT_VER_MINOR
CFLAGS += -DFT_VER_BUILD

INCS += $(NO-OS)/drivers/platform/ftd2xx/mpsse/ftd2xx_platform.h \
        $(NO-OS)/drivers/platform/ftd2xx/mpsse/ftd2xx_uart.h

SRCS += $(NO-OS)/drivers/platform/ftd2xx/mpsse/ftd2xx_platform.c \
        $(NO-OS)/drivers/platform/ftd2xx/mpsse/ftd2xx_uart.c

SRC_DIRS += $(NO-OS)/libraries/ftd2xx/release/include   \
            $(NO-OS)/libraries/ftd2xx/release/source    \
            $(NO-OS)/libraries/ftd2xx/release/libftd2xx
