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

SRCS += $(PROJECT)/src/ad738x_fmc.c
SRCS += $(DRIVERS)/adc/ad738x/ad738x.c \
	$(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c \
	$(DRIVERS)/axi_core/spi_engine/spi_engine.c \
	$(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c
SRCS +=	$(PLATFORM_DRIVERS)/xilinx_axi_io.c \
	$(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(PLATFORM_DRIVERS)/xilinx_delay.c
INCS += $(PROJECT)/src/app_config.h \
	$(PROJECT)/src/parameters.h \
	$(DRIVERS)/adc/ad738x/ad738x.h \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h \
	$(DRIVERS)/axi_core/spi_engine/spi_engine.h \
	$(DRIVERS)/axi_core/spi_engine/spi_engine_private.h
INCS +=	$(PLATFORM_DRIVERS)/$(PLATFORM)_spi.h					
INCS +=	$(INCLUDE)/no_os_axi_io.h \
	$(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_mutex.h 
