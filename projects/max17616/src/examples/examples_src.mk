# Ensure exactly one of BASIC_EXAMPLE or IIO_EXAMPLE is selected
ifeq (,$(filter y,$(strip $(IIO_EXAMPLE)) $(strip $(BASIC_EXAMPLE))))
$(error You must select exactly one example: BASIC_EXAMPLE or IIO_EXAMPLE)
endif

# Count how many examples are selected
example_count := 0
ifneq (,$(findstring y,$(strip $(IIO_EXAMPLE))))
example_count := $(shell expr $(example_count) + 1)
endif
ifneq (,$(findstring y,$(strip $(BASIC_EXAMPLE))))
example_count := $(shell expr $(example_count) + 1)
endif

# Enforce mutual exclusivity
ifneq (1,$(example_count))
$(error Only one example can be selected at a time)
endif

ifeq (y,$(strip $(IIO_EXAMPLE)))
IIOD=y
CFLAGS += -DIIO_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h
endif

ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/basic/basic_example.c
INCS += $(PROJECT)/src/examples/basic/basic_example.h
endif

ifeq (y, $(strip $(IIOD)))
LIBRARIES += iio
SRCS += $(NO-OS)/iio/iio_app/iio_app.c	\
	$(NO-OS)/iio/iio.c	\
	$(NO-OS)/iio/iiod.c	\
	$(NO-OS)/util/no_os_fifo.c

INCS += $(NO-OS)/iio/iio_app/iio_app.h	\
	$(NO-OS)/iio/iio.h	\
	$(NO-OS)/iio/iiod.h	\
	$(NO-OS)/iio/iio_types.h	\
	$(NO-OS)/include/no_os_fifo.h

ifeq (y,$(strip $(IIO_EXAMPLE)))
SRCS += $(DRIVERS)/power/max17616/iio_max17616.c
INCS += $(DRIVERS)/power/max17616/iio_max17616.h
endif
endif
