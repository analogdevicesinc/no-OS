# Project includes + source code
SRCS := $(PROJECT)/src/common_data.c

NO_OS_INC_DIRS := \
	$(INCLUDE) \
	$(PROJECT)/src/ \
	$(PLATFORM_DRIVERS) \
	$(DRIVERS)/api/ \
	$(DRIVERS)/axi_core/axi_dmac/ \
	$(DRIVERS)/axi_core/spi_engine/ \
	$(DRIVERS)/axi_core/axi_pwmgen/ \
	$(DRIVERS)/axi_core/clk_axi_clkgen/ \
	$(DRIVERS)/axi_core/axi_adc_core/ \
	$(DRIVERS)/adc/ad7606/

ifeq (y,$(strip $(IIO_EXAMPLE)))
IIOD=y
SRCS += $(PROJECT)/src/iio_example.c

NO_OS_INC_DIRS += \
        $(NO-OS)/iio/iio_app/

SRCS += $(DRIVERS)/adc/ad7606/iio_ad7606.c \
        $(PLATFORM_DRIVERS)/xilinx_uart.c \
        $(PLATFORM_DRIVERS)/xilinx_irq.c \
        $(NO-OS)/iio/iio_app/iio_app.c

else ifeq (y,$(strip $(BASIC_EXAMPLE)))
SRCS += $(PROJECT)/src/basic_example.c
endif

# The driver targeted by this project
SRCS += $(DRIVERS)/adc/ad7606/ad7606.c

SRCS += $(DRIVERS)/api/no_os_uart.c     \
        $(NO-OS)/util/no_os_fifo.c      \
        $(NO-OS)/util/no_os_list.c      \
        $(NO-OS)/util/no_os_util.c      \
        $(NO-OS)/util/no_os_alloc.c     \
        $(NO-OS)/util/no_os_mutex.c     \
        $(DRIVERS)/api/no_os_gpio.c     \
        $(DRIVERS)/api/no_os_irq.c      \
        $(NO-OS)/util/no_os_crc8.c      \
        $(NO-OS)/util/no_os_crc16.c     \
        $(NO-OS)/util/no_os_crc24.c     \
        $(NO-OS)/util/no_os_util.c


SRCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c \
        $(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.c \
        $(DRIVERS)/axi_core/axi_pwmgen/axi_pwm.c \
        $(DRIVERS)/axi_core/axi_dmac/axi_dmac.c \
        $(DRIVERS)/axi_core/spi_engine/spi_engine.c \
        $(DRIVERS)/api/no_os_spi.c \
        $(DRIVERS)/api/no_os_pwm.c

SRCS += $(PLATFORM_DRIVERS)/xilinx_axi_io.c \
        $(PLATFORM_DRIVERS)/xilinx_gpio.c \
        $(PLATFORM_DRIVERS)/xilinx_delay.c
