ifeq (y,$(strip $(IIO_EXAMPLE)))
IIOD=y
CFLAGS += -DIIO_EXAMPLE
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h
endif

ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE
SRCS += $(PROJECT)/src/examples/basic/basic_example.c
INCS += $(PROJECT)/src/examples/basic/basic_example.h
endif

ifeq (y, $(strip $(IIOD)))
LIBRARIES += iio
SRCS += $(NO-OS)/iio/iio_app/iio_app.c	\
	$(DRIVERS)/power/adp1050/iio_adp1050.c	\
	$(NO-OS)/iio/iio.c	\
	$(NO-OS)/iio/iiod.c	\
	$(NO-OS)/util/no_os_fifo.c

INCS += $(NO-OS)/iio/iio_app/iio_app.h	\
	$(DRIVERS)/power/adp1050/iio_adp1050.h	\
	$(NO-OS)/iio/iio.h	\
	$(NO-OS)/iio/iiod.h	\
	$(NO-OS)/iio/iio_types.h	\
	$(NO-OS)/include/no_os_fifo.h
endif
