#See No-OS/tool/scripts/src_model.mk for variable description

SRC_DIRS += $(PROJECT)/srcs			\
		$(NO-OS)/drivers/dac/ad3552r	\
		$(NO-OS)/drivers/spi		\
		$(NO-OS)/drivers/gpio		\
		$(PLATFORM_DRIVERS)		\
		$(NO-OS)/iio/iio_app		\
		$(NO-OS)/iio/iio_trigger	\
		$(NO-OS)/util			\
		$(INCLUDE)


ifeq '$(PLATFORM)' 'linux'
SRC_DIRS += $(NO-OS)/network
IGNORED_FILES = $(NO-OS)/network/wifi

DISABLE_SECURE_SOCKET ?= y
ENABLE_IIO_NETWORK = y
endif