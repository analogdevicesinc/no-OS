TINYIIOD ?= n
REVISION = B0
ifeq ($(REVISION),B0)
CFLAGS_REVISION = -DSI_REV_B0 \
		  -DADI_ADRV9001_ARM_BINARY_IMAGE_FILE_SIZE_BYTES=262144
endif
CFLAGS = -DADI_DYNAMIC_PROFILE_LOAD \
	 -DADI_COMMON_VERBOSE=1 \
	 -DADI_ADRV9001_ARM_VERBOSE \
	 -DADI_VALIDATE_PARAMS \
	 $(CFLAGS_REVISION)

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk
