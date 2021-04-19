#See No-OS/tool/scripts/src_model.mk for variable description

SRC_DIRS += $(PROJECT)/src
SRC_DIRS += $(DRIVERS)/amplifiers/ada4250
SRC_DIRS += $(NO-OS)/iio/iio_app

SRC_DIRS += $(PLATFORM_DRIVERS)
SRC_DIRS += $(INCLUDE)

SRCS += $(NO-OS)/util/util.c				\
	$(NO-OS)/util/list.c

IGNORED_FILES += $(PLATFORM_DRIVERS)/uart_stdio.c
IGNORED_FILES += $(PLATFORM_DRIVERS)/uart_stdio.h

TINYIIOD=y
