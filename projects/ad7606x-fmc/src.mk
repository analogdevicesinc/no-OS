# Project includes + source code
INCS := $(PROJECT)/src/parameters.h      \
        $(PROJECT)/src/common_data.h
SRCS := $(PROJECT)/src/common_data.c

ifeq (y,$(strip $(IIO_EXAMPLE)))
IIOD=y
SRCS += $(PROJECT)/src/iio_example.c

INCS += $(DRIVERS)/adc/ad7606/iio_ad7606.h \
        $(PLATFORM_DRIVERS)/xilinx_uart.h \
        $(PLATFORM_DRIVERS)/xilinx_irq.h \
        $(NO-OS)/iio/iio_app/iio_app.h
SRCS += $(DRIVERS)/adc/ad7606/iio_ad7606.c \
        $(PLATFORM_DRIVERS)/xilinx_uart.c \
        $(PLATFORM_DRIVERS)/xilinx_irq.c \
        $(NO-OS)/iio/iio_app/iio_app.c

else ifeq (y,$(strip $(BASIC_EXAMPLE)))
SRCS += $(PROJECT)/src/basic_example.c
endif

# The driver targeted by this project
INCS += $(DRIVERS)/adc/ad7606/ad7606.h
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


INCS += $(INCLUDE)/no_os_delay.h     \
        $(INCLUDE)/no_os_error.h     \
        $(INCLUDE)/no_os_fifo.h      \
        $(INCLUDE)/no_os_irq.h       \
        $(INCLUDE)/no_os_lf256fifo.h \
        $(INCLUDE)/no_os_list.h      \
        $(INCLUDE)/no_os_dma.h       \
        $(INCLUDE)/no_os_timer.h     \
        $(INCLUDE)/no_os_uart.h      \
        $(INCLUDE)/no_os_util.h      \
        $(INCLUDE)/no_os_alloc.h     \
        $(INCLUDE)/no_os_mutex.h     \
        $(INCLUDE)/no_os_pwm.h       \
        $(INCLUDE)/no_os_print_log.h \
        $(INCLUDE)/no_os_axi_io.h    \
        $(INCLUDE)/no_os_gpio.h      \
        $(INCLUDE)/no_os_spi.h       \
        $(INCLUDE)/no_os_crc.h       \
        $(INCLUDE)/no_os_crc8.h      \
        $(INCLUDE)/no_os_crc16.h     \
        $(INCLUDE)/no_os_crc24.h     \
        $(INCLUDE)/no_os_print_log.h

INCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h \
        $(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.h \
        $(DRIVERS)/axi_core/axi_pwmgen/axi_pwm_extra.h \
        $(DRIVERS)/axi_core/spi_engine/spi_engine.h \
        $(DRIVERS)/axi_core/spi_engine/spi_engine_private.h \
        $(DRIVERS)/axi_core/axi_dmac/axi_dmac.h

SRCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c \
        $(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.c \
        $(DRIVERS)/axi_core/axi_pwmgen/axi_pwm.c \
        $(DRIVERS)/axi_core/axi_dmac/axi_dmac.c \
        $(DRIVERS)/axi_core/spi_engine/spi_engine.c \
        $(DRIVERS)/api/no_os_spi.c \
        $(DRIVERS)/api/no_os_pwm.c

INCS += $(PLATFORM_DRIVERS)/xilinx_gpio.h \
        $(PLATFORM_DRIVERS)/xilinx_spi.h
SRCS += $(PLATFORM_DRIVERS)/xilinx_axi_io.c \
        $(PLATFORM_DRIVERS)/xilinx_gpio.c \
        $(PLATFORM_DRIVERS)/xilinx_delay.c
