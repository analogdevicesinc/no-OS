/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ad469x_fmcz examples.
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "common_data.h"
#include "no_os_pwm.h"
#include "no_os_spi.h"
#include "no_os_uart.h"
#include "ad469x.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
struct no_os_uart_init_param iio_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS,
};

struct no_os_spi_init_param spi_init = {
	.chip_select = SPI_CS,
	.max_speed_hz = SPI_BAUDRATE,
	.mode = NO_OS_SPI_MODE_3,
	.platform_ops = SPI_OPS,
	.extra = (void*)SPI_EXTRA,
};
struct spi_engine_offload_init_param spi_engine_offload_init_param = {
	.offload_config = OFFLOAD_RX_EN,
	.rx_dma_baseaddr = DMA_BASEADDR,
};

struct axi_clkgen_init clkgen_init = {
	.name = "rx_clkgen",
	.base = RX_CLKGEN_BASEADDR,
	.parent_rate = 100000000,
};

struct no_os_pwm_init_param trigger_pwm_init = {
#if (defined(AD4696_DEV) || defined(AD4698_DEV))
	.period_ns = 1000,
	.duty_cycle_ns = 10,
#elif (defined(AD4695_DEV) || defined(AD4697_DEV))
	.period_ns = 2000,
	.duty_cycle_ns = 20,
#endif
	.polarity = NO_OS_PWM_POLARITY_HIGH,
	.platform_ops = PWM_OPS,
	.extra = PWM_EXTRA,
};

struct no_os_gpio_init_param ad469x_resetn = {
	.number = GPIO_RESETN_1,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct ad469x_init_param ad469x_init_param = {
#ifdef AD4695_DEV
	.dev_id = ID_AD4695,
#elif defined(AD4696_DEV)
	.dev_id = ID_AD4696,
#elif defined(AD4697_DEV)
	.dev_id = ID_AD4697,
#elif defined(AD4698_DEV)
	.dev_id = ID_AD4698,
#endif
#if (defined(AD4695_DEV) || defined(AD4696_DEV))
	.num_data_ch = 16,
#elif (defined(AD4697_DEV) || defined(AD4698_DEV))
	.num_data_ch = 8,
#endif
	.spi_init = &spi_init,
	.offload_init_param = &spi_engine_offload_init_param,
	.trigger_pwm_init = &trigger_pwm_init,
	.gpio_resetn = &ad469x_resetn,
	.clkgen_init = &clkgen_init,
	.axi_clkgen_rate = 160000000,
	.reg_access_speed = 20000000,
	.reg_data_width = 8,
	.std_seq_pin_pairing = AD469x_INx_COM,
	.capture_data_width = 16,
	.std_seq_osr = AD469x_OSR_1,
#if defined(AD469X_SEQ_ADVANCED)
	.ch_sequence = AD469x_advanced_seq,
	.adv_seq_osr_resol[0] = AD469x_OSR_1,
	.adv_seq_osr_resol[1] = AD469x_OSR_1,
	.adv_seq_osr_resol[2] = AD469x_OSR_1,
	.adv_seq_osr_resol[3] = AD469x_OSR_1,
	.adv_seq_osr_resol[4] = AD469x_OSR_1,
	.adv_seq_osr_resol[5] = AD469x_OSR_1,
	.adv_seq_osr_resol[6] = AD469x_OSR_1,
	.adv_seq_osr_resol[7] = AD469x_OSR_1,
	.adv_seq_osr_resol[8] = AD469x_OSR_1,
	.adv_seq_osr_resol[9] = AD469x_OSR_1,
	.adv_seq_osr_resol[10] = AD469x_OSR_1,
	.adv_seq_osr_resol[11] = AD469x_OSR_1,
	.adv_seq_osr_resol[12] = AD469x_OSR_1,
	.adv_seq_osr_resol[13] = AD469x_OSR_1,
	.adv_seq_osr_resol[14] = AD469x_OSR_1,
	.adv_seq_osr_resol[15] = AD469x_OSR_1,
#elif (defined(AD469X_SEQ_STANDARD) || defined (IIO_EXAMPLE))
	.ch_sequence = AD469x_standard_seq,
#elif defined(AD469X_SEQ_SINGLE)
	.ch_sequence = AD469x_single_cycle,
#endif
#ifdef AD469X_TEMP_ENABLE
	.temp_enabled = true,
#else
	.temp_enabled = false,
#endif
	.enable_extended_init = true,
	.dcache_invalidate_range = (void (*)(uint32_t,  uint32_t))DCACHE_INVALIDATE,
};
