################################################################################
#									       #
#     Shared variables:							       #
#	- PROJECT							       #
#	- DRIVERS							       #
#	- INCLUDE							       #
#	- PLATFORM_DRIVERS						       #
#	- NO-OS								       #
#									       #
################################################################################

SRCS += $(PROJECT)/src/adf5902_sdz.c
SRCS += $(DRIVERS)/spi/spi.c						\
	$(DRIVERS)/gpio/gpio.c							\
	$(DRIVERS)/frequency/adf5902/adf5902.c
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c				\
	$(PLATFORM_DRIVERS)/xilinx_spi.c				\
	$(PLATFORM_DRIVERS)/xilinx_gpio.c				\
	$(PLATFORM_DRIVERS)/delay.c
INCS +=	$(PROJECT)/src/app_config.h					\
	$(PROJECT)/src/parameters.h
INCS += $(DRIVERS)/frequency/adf5902/adf5902.h
INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h				\
	$(PLATFORM_DRIVERS)/gpio_extra.h
INCS +=	$(INCLUDE)/axi_io.h							\
	$(INCLUDE)/spi.h								\
	$(INCLUDE)/gpio.h								\
	$(INCLUDE)/error.h								\
	$(INCLUDE)/delay.h								\
	$(INCLUDE)/util.h								\
	$(INCLUDE)/print_log.h
