#See No-OS/tool/scripts/src_model.mk for variable description

SRC_DIRS += $(PROJECT)/srcs			\
		$(NO-OS)/drivers/api		\
		$(PLATFORM_DRIVERS)		\
		$(NO-OS)/util			\
		$(INCLUDE)
SRCS += $(NO-OS)/drivers/dac/ad3552r/ad3552r.c
INCS += $(NO-OS)/drivers/dac/ad3552r/ad3552r.h

ifeq (y,$(strip $(IIOD)))
SRC_DIRS += $(NO-OS)/iio/iio_app
SRCS += $(NO-OS)/drivers/dac/ad3552r/iio_ad3552r.c
INCS += $(NO-OS)/drivers/dac/ad3552r/iio_ad3552r.h
endif
