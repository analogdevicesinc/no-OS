ifeq (y,$(strip $(GENERIC_SPI_EXAMPLE)))
CFLAGS += -DGENERIC_SPI_EXAMPLE
SRCS += $(PROJECT)/src/examples/generic_spi/generic_spi_example.c
INCS += $(PROJECT)/src/examples/generic_spi/generic_spi_example.h

INCS += $(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_delay.h
endif

ifeq (y,$(strip $(AXI_QSPI_EXAMPLE)))
CFLAGS += -DAXI_QSPI_EXAMPLE
SRCS += $(PROJECT)/src/examples/axi_qspi/axi_qspi_example.c
INCS += $(PROJECT)/src/examples/axi_qspi/axi_qspi_example.h
endif
