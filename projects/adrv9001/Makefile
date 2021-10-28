TINYIIOD ?= n
CFLAGS = -DADI_DYNAMIC_PROFILE_LOAD \
	 -DADI_COMMON_VERBOSE=1 \
	 -DADI_ADRV9001_ARM_VERBOSE \
	 -DADI_VALIDATE_PARAMS \
	 $(CFLAGS_REVISION)

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk
