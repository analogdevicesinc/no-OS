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

SRCS := $(PROJECT)/src/ad9265_fmc_125ebz.c
SRCS += $(DRIVERS)/adc/ad9265/ad9265.c					\
	$(DRIVERS)/spi/spi.c						\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c				\
	$(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c			\
	$(NO-OS)/util/util.c
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c					\
	$(PLATFORM_DRIVERS)/xilinx_spi.c				\
	$(PLATFORM_DRIVERS)/delay.c
INCS += $(PROJECT)/src/parameters.h					\
	$(DRIVERS)/adc/ad9265/ad9265.h					\
	$(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h			\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h
INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h
INCS +=	$(INCLUDE)/axi_io.h						\
	$(INCLUDE)/spi.h						\
	$(INCLUDE)/error.h						\
	$(INCLUDE)/delay.h						\
	$(INCLUDE)/print_log.h						\
	$(INCLUDE)/util.h