TARGET := iio_demo
TINYIIOD ?= y
ifeq (aducm3029,$(strip $(PLATFORM)))
SRC_DIRS += projects/$(TARGET)/src
SRC_DIRS += iio/iio_app
SRC_DIRS += iio/iio_demo
ifeq (y,$(strip $(TINYIIOD)))
SRC_DIRS += libraries/iio
endif
include ../../tools/scripts/aducm.mk
else
ifeq ($(OS), Windows_NT)
include ../../tools/scripts/windows.mk
else
include ../../tools/scripts/linux.mk
endif
endif
