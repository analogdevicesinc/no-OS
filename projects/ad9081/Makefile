TARGET := ad9081
TINYIIOD = y
QUAD_MXFE = n
ifeq ($(OS), Windows_NT)
include ../../tools/scripts/windows.mk
else
include ../../tools/scripts/linux.mk
endif
ifeq (y,$(strip $(TINYIIOD)))
	CFLAGS += -D IIO_EXAMPLE
endif
ifeq (y,$(strip $(QUAD_MXFE)))
	CFLAGS += -D QUAD_MXFE
endif
