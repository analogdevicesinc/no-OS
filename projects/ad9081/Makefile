TARGET := ad9081
TINYIIOD = y
ifeq ($(OS), Windows_NT)
include ../../tools/scripts/windows.mk
else
include ../../tools/scripts/linux.mk
endif
ifeq (y,$(strip $(TINYIIOD)))
	CFLAGS += -D IIO_SUPPORT
endif
