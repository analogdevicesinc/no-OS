TARGET := adrv9001
TINYIIOD ?= n
CFLAGS = -DSI_REV_B0 \
	 -DADI_DYNAMIC_PROFILE_LOAD \
	 -DADI_COMMON_VERBOSE=1 \
	 -DADI_ADRV9001_ARM_VERBOSE \
	 -DADI_VALIDATE_PARAMS
ifeq ($(OS), Windows_NT)
include ../../tools/scripts/windows.mk
else
include ../../tools/scripts/linux.mk
endif
