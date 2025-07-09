include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk
include $(PROJECT)/src/examples/examples_src.mk

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c

INCS += $(PROJECT)/src/common/common_data.h
SRCS += $(PROJECT)/src/common/common_data.c

INCS += $(PROJECT)/src/platform/platform_includes.h

INCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.h
SRCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.c

SRCS += $(DRIVERS)/api/no_os_uart.c     	\
        $(DRIVERS)/api/no_os_gpio.c     	\
	$(DRIVERS)/api/no_os_spi.c  		\
	$(DRIVERS)/api/no_os_irq.c 		\
	$(DRIVERS)/api/no_os_timer.c		\
        $(DRIVERS)/api/no_os_dma.c      	\
        $(NO-OS)/util/no_os_fifo.c      	\
	$(NO-OS)/util/no_os_mutex.c     	\
	$(NO-OS)/util/no_os_lf256fifo.c 	\
        $(NO-OS)/util/no_os_list.c      	\
        $(NO-OS)/util/no_os_util.c      	\
        $(NO-OS)/util/no_os_alloc.c		\
	$(NO-OS)/util/no_os_clk.c

INCS += $(INCLUDE)/no_os_delay.h     		\
        $(INCLUDE)/no_os_error.h     		\
	$(INCLUDE)/no_os_gpio.h      		\
	$(INCLUDE)/no_os_mutex.h     		\
	$(INCLUDE)/no_os_spi.h       		\
        $(INCLUDE)/no_os_fifo.h      		\
        $(INCLUDE)/no_os_irq.h       		\
        $(INCLUDE)/no_os_lf256fifo.h 		\
        $(INCLUDE)/no_os_list.h      		\
	$(INCLUDE)/no_os_print_log.h 		\
        $(INCLUDE)/no_os_timer.h     		\
        $(INCLUDE)/no_os_uart.h      		\
        $(INCLUDE)/no_os_util.h      		\
        $(INCLUDE)/no_os_units.h     		\
        $(INCLUDE)/no_os_dma.h       		\
	$(INCLUDE)/no_os_clk.h 			\
	$(INCLUDE)/no_os_axi_io.h 		\
        $(INCLUDE)/no_os_alloc.h
		
INCS += $(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.h 	\
	$(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h 	\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h 		\
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.h 	\
	$(DRIVERS)/axi_core/jesd204/axi_adxcvr.h 		\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.h 		\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.h 		\
	$(DRIVERS)/axi_core/jesd204/jesd204_clk.h 		\
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.h	\
	$(INCLUDE)/jesd204.h					\
	$(NO-OS)/jesd204/jesd204-priv.h

SRCS += $(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.c 	\
	$(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c 	\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c			\
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.c 	\
	$(DRIVERS)/axi_core/jesd204/axi_adxcvr.c 		\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.c 		\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.c 		\
	$(DRIVERS)/axi_core/jesd204/jesd204_clk.c 		\
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.c 	\
	$(NO-OS)/jesd204/jesd204-core.c 			\
	$(NO-OS)/jesd204/jesd204-fsm.c

SRC_DIRS += $(DRIVERS)/rf-transceiver/apollo			\
	    $(DRIVERS)/rf-transceiver/apollo/firmware

INCS += $(DRIVERS)/frequency/adf4382/adf4382.h			\
	$(DRIVERS)/frequency/hmc7044/hmc7044.h			\
	$(DRIVERS)/rf-transceiver/apollo/ad9088.h
	
SRCS += $(DRIVERS)/frequency/adf4382/adf4382.c			\
	$(DRIVERS)/frequency/hmc7044/hmc7044.c			\
	$(DRIVERS)/rf-transceiver/apollo/ad9088.c
