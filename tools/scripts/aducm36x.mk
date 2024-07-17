
CC = arm-none-eabi-gcc
SIZE = arm-none-eabi-size

PROJECT_BUILD = $(BUILD_DIR)

TARGET ?= aducm360
TARGET_NUMBER := $(word 2,$(subst m, ,$(subst M, ,$(TARGET))))
TARGET_MIXED_CASE = $(addprefix ADuCM,$(TARGET_NUMBER))

INCS += $(NO-OS)/libraries/cmsis/device/$(TARGET_MIXED_CASE)/$(TARGET_MIXED_CASE).h
INCS += $(NO-OS)/libraries/cmsis/device/$(TARGET_MIXED_CASE)/system_$(TARGET_MIXED_CASE).h
INCS += $(NO-OS)/libraries/cmsis/CMSIS_5/CMSIS/Core/Include/core_cm3.h
INCS += $(NO-OS)/libraries/cmsis/CMSIS_5/CMSIS/Core/Include/cmsis_version.h
INCS += $(NO-OS)/libraries/cmsis/CMSIS_5/CMSIS/Core/Include/cmsis_compiler.h
INCS += $(NO-OS)/libraries/cmsis/CMSIS_5/CMSIS/Core/Include/cmsis_gcc.h

SRCS += $(NO-OS)/libraries/cmsis/device/$(TARGET_MIXED_CASE)/startup_$(TARGET_MIXED_CASE).c
SRCS += $(NO-OS)/libraries/cmsis/device/$(TARGET_MIXED_CASE)/system_$(TARGET_MIXED_CASE).c

LSCRIPT = $(NO-OS)/libraries/cmsis/device/$(TARGET_MIXED_CASE)/$(TARGET_MIXED_CASE).ld

CFLAGS += -D_start=main
CFLAGS += -g3
CFLAGS += -mcpu=cortex-m3

LDFLAGS = --specs=nosys.specs

$(PLATFORM)_project:
	$(call mk_dir, $(BUILD_DIR)) $(HIDE)

$(PLATFORM)_reset:

$(PLATFORM)_post_build:
