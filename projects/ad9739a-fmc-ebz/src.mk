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

SRCS := $(PROJECT)/src/ad9739a_fmc_ebz.c
SRCS += $(DRIVERS)/dac/ad9739a/ad9739a.c				\
	$(DRIVERS)/frequency/adf4350/adf4350.c				\
	$(DRIVERS)/spi/spi.c						\
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.c			\
	$(NO-OS)/util/util.c
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c					\
	$(PLATFORM_DRIVERS)/xilinx_spi.c				\
	$(PLATFORM_DRIVERS)/delay.c
INCS += $(PROJECT)/src/parameters.h					\
	$(DRIVERS)/frequency/adf4350/adf4350.h				\
	$(DRIVERS)/dac/ad9739a/ad9739a.h				\
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.h
INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h					
INCS +=	$(INCLUDE)/axi_io.h						\
	$(INCLUDE)/spi.h						\
	$(INCLUDE)/error.h						\
	$(INCLUDE)/delay.h						\
	$(INCLUDE)/print_log.h						\
	$(INCLUDE)/util.h
