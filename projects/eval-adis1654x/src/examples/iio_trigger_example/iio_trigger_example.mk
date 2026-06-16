IIOD=y
INCS += $(DRIVERS)/imu/iio_adis_internals.h
SRCS += $(DRIVERS)/imu/iio_adis.c
SRCS += $(DRIVERS)/imu/iio_adis_trig.c
INCS += $(DRIVERS)/imu/iio_adis1654x.h
SRCS += $(DRIVERS)/imu/iio_adis1654x.c
SRCS += $(NO-OS)/iio/iio_trigger.c
INCS += $(NO-OS)/iio/iio_trigger.h
