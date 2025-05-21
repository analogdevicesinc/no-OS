ifeq ($(wildcard $(NO-OS)/libraries/ftd2xx/release),)
        UNAME_S := $(shell uname -s)
        ifeq ($(UNAME_S),Darwin)
                SCRIPT_CFG := $(shell mkdir -p $(NO-OS)/libraries/ftd2xx)
                SCRIPT_CFG := $(shell python3 $(NO-OS)/tools/scripts/config_ftd2xx.py $(NO-OS)/libraries/ftd2xx)
        else
                SCRIPT_CFG := $(shell mkdir -p $(NO-OS)/libraries/ftd2xx)
                SCRIPT_CFG := $(shell python $(NO-OS)/tools/scripts/config_ftd2xx.py $(NO-OS)/libraries/ftd2xx)
        endif
endif

CFLAGS += -DFT_VER_MAJOR
CFLAGS += -DFT_VER_MINOR
CFLAGS += -DFT_VER_BUILD

INCS += $(NO-OS)/drivers/platform/ftd2xx/mpsse/ftd2xx_platform.h \
        $(NO-OS)/drivers/platform/ftd2xx/mpsse/ftd2xx_uart.h

SRCS += $(NO-OS)/drivers/platform/ftd2xx/mpsse/ftd2xx_platform.c \
        $(NO-OS)/drivers/platform/ftd2xx/mpsse/ftd2xx_uart.c

ifeq ($(OS), Windows_NT)
CFLAGS += -DFTDIMPSSE_STATIC
SRC_DIRS += $(NO-OS)/libraries/ftd2xx/release/include   \
            $(NO-OS)/libraries/ftd2xx/release/libftd2xx
INCS += $(NO-OS)/libraries/ftd2xx/release/source/ftdi_common.h  \
        $(NO-OS)/libraries/ftd2xx/release/source/ftdi_infra.h   \
        $(NO-OS)/libraries/ftd2xx/release/source/ftdi_mid.h
SRCS += $(NO-OS)/libraries/ftd2xx/release/source/ftdi_i2c.c     \
        $(NO-OS)/libraries/ftd2xx/release/source/ftdi_infra.c   \
        $(NO-OS)/libraries/ftd2xx/release/source/ftdi_mid.c     \
        $(NO-OS)/libraries/ftd2xx/release/source/ftdi_spi.c
else
SRC_DIRS += $(NO-OS)/libraries/ftd2xx/release/include   \
            $(NO-OS)/libraries/ftd2xx/release/source    \
            $(NO-OS)/libraries/ftd2xx/release/libftd2xx
endif
