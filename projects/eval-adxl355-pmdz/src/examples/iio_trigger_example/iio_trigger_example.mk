IIOD = y
INCS += $(DRIVERS)/accel/adxl355/iio_adxl355.h
SRCS += $(DRIVERS)/accel/adxl355/iio_adxl355.c
SRCS += $(PROJECT)/src/examples/iio_trigger_example/iio_trigger_example.c
SRCS += $(NO-OS)/iio/iio_trigger.c
INCS += $(NO-OS)/iio/iio_trigger.h
SRCS += $(DRIVERS)/accel/adxl355/iio_adxl355_trig.c
