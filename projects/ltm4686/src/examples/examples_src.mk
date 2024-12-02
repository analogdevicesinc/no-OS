ifeq (y,$(strip $(IIO_EXAMPLE)))
IIOD=y
CFLAGS += -DIIO_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
endif

ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/basic/basic_example.c
endif

ifeq (y, $(strip $(IIOD)))
LIBRARIES += iio
NO_OS_INC_DIRS += \
	$(NO-OS)/iio/

SRCS += $(NO-OS)/iio/iio_app/iio_app.c	\
	$(DRIVERS)/power/ltm4686/iio_ltm4686.c	\
	$(NO-OS)/iio/iio.c \
	$(NO-OS)/iio/iiod.c \
	$(NO-OS)/util/no_os_fifo.c
endif
