#See No-OS/tool/scripts/src_model.mk for variable description
PLATFORM = aducm3029

# TINYIIOD=y

SRC_DIRS += $(PROJECT)/src \
		$(PLATFORM_DRIVERS)\
		$(NO-OS)/util\
		$(NO-OS)/include \
		$(NO-OS)/iio/iio_app
