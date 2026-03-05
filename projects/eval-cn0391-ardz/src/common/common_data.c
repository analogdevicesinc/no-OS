/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by eval-cn0391-ardz examples.
 *   @author Mircea Vlasin (mircea.vlasin@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "common_data.h"
#include "parameters.h"
#include "no_os_spi.h"
#include "no_os_uart.h"

struct no_os_uart_init_param cn0391_uart_ip = {
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

struct no_os_spi_init_param cn0391_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.chip_select = SPI_CS,
	.max_speed_hz = SPI_BAUDRATE,
	.mode = NO_OS_SPI_MODE_3,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA,
};

/*
 * CN0391-specific AD7124 register defaults.
 * ADC_Control: Full power, internal ref enabled, continuous mode, internal clk
 * IOCon1: IOUT0=750µA, pin switched per channel at read time (cn0391_read_temperature)
 * Filter: CN0391_FILTER_TYPE (0=Sinc4, 2=Sinc3), FS=384 (~50 SPS single-channel)
 */
static struct ad7124_st_reg cn0391_ad7124_regs[AD7124_REG_NO] = {
	{0x00, 0x00,     1, 2}, /* AD7124_Status */
	{0x01, 0x05C0,   2, 1}, /* AD7124_ADC_Control: full power, ref_en, cont, int clk */
	{0x02, 0x0000,   3, 2}, /* AD7124_Data */
	{0x03, 0x000000, 3, 1}, /* AD7124_IOCon1 */
	{0x04, 0x0000,   2, 1}, /* AD7124_IOCon2 */
	{0x05, 0x02,     1, 2}, /* AD7124_ID */
	{0x06, 0x0000,   3, 2}, /* AD7124_Error */
	{0x07, 0x0040,   3, 1}, /* AD7124_Error_En */
	{0x08, 0x00,     1, 2}, /* AD7124_Mclk_Count */
	{0x09, 0x8001,   2, 1}, /* AD7124_Channel_0 */
	{0x0A, 0x0001,   2, 1}, /* AD7124_Channel_1 */
	{0x0B, 0x0001,   2, 1}, /* AD7124_Channel_2 */
	{0x0C, 0x0001,   2, 1}, /* AD7124_Channel_3 */
	{0x0D, 0x0001,   2, 1}, /* AD7124_Channel_4 */
	{0x0E, 0x0001,   2, 1}, /* AD7124_Channel_5 */
	{0x0F, 0x0001,   2, 1}, /* AD7124_Channel_6 */
	{0x10, 0x0001,   2, 1}, /* AD7124_Channel_7 */
	{0x11, 0x0001,   2, 1}, /* AD7124_Channel_8 */
	{0x12, 0x0001,   2, 1}, /* AD7124_Channel_9 */
	{0x13, 0x0001,   2, 1}, /* AD7124_Channel_10 */
	{0x14, 0x0001,   2, 1}, /* AD7124_Channel_11 */
	{0x15, 0x0001,   2, 1}, /* AD7124_Channel_12 */
	{0x16, 0x0001,   2, 1}, /* AD7124_Channel_13 */
	{0x17, 0x0001,   2, 1}, /* AD7124_Channel_14 */
	{0x18, 0x0001,   2, 1}, /* AD7124_Channel_15 */
	{0x19, 0x0860,   2, 1}, /* AD7124_Config_0 */
	{0x1A, 0x0860,   2, 1}, /* AD7124_Config_1 */
	{0x1B, 0x0860,   2, 1}, /* AD7124_Config_2 */
	{0x1C, 0x0860,   2, 1}, /* AD7124_Config_3 */
	{0x1D, 0x0860,   2, 1}, /* AD7124_Config_4 */
	{0x1E, 0x0860,   2, 1}, /* AD7124_Config_5 */
	{0x1F, 0x0860,   2, 1}, /* AD7124_Config_6 */
	{0x20, 0x0860,   2, 1}, /* AD7124_Config_7 */
	{0x21, CN0391_FILTER_REG_VAL, 3, 1}, /* AD7124_Filter_0 */
	{0x22, CN0391_FILTER_REG_VAL, 3, 1}, /* AD7124_Filter_1 */
	{0x23, CN0391_FILTER_REG_VAL, 3, 1}, /* AD7124_Filter_2 */
	{0x24, CN0391_FILTER_REG_VAL, 3, 1}, /* AD7124_Filter_3 */
	{0x25, CN0391_FILTER_REG_VAL, 3, 1}, /* AD7124_Filter_4 */
	{0x26, CN0391_FILTER_REG_VAL, 3, 1}, /* AD7124_Filter_5 */
	{0x27, CN0391_FILTER_REG_VAL, 3, 1}, /* AD7124_Filter_6 */
	{0x28, CN0391_FILTER_REG_VAL, 3, 1}, /* AD7124_Filter_7 */
	{0x29, 0x800000, 3, 1}, /* AD7124_Offset_0 */
	{0x2A, 0x800000, 3, 1}, /* AD7124_Offset_1 */
	{0x2B, 0x800000, 3, 1}, /* AD7124_Offset_2 */
	{0x2C, 0x800000, 3, 1}, /* AD7124_Offset_3 */
	{0x2D, 0x800000, 3, 1}, /* AD7124_Offset_4 */
	{0x2E, 0x800000, 3, 1}, /* AD7124_Offset_5 */
	{0x2F, 0x800000, 3, 1}, /* AD7124_Offset_6 */
	{0x30, 0x800000, 3, 1}, /* AD7124_Offset_7 */
	{0x31, 0x500000, 3, 1}, /* AD7124_Gain_0 */
	{0x32, 0x500000, 3, 1}, /* AD7124_Gain_1 */
	{0x33, 0x500000, 3, 1}, /* AD7124_Gain_2 */
	{0x34, 0x500000, 3, 1}, /* AD7124_Gain_3 */
	{0x35, 0x500000, 3, 1}, /* AD7124_Gain_4 */
	{0x36, 0x500000, 3, 1}, /* AD7124_Gain_5 */
	{0x37, 0x500000, 3, 1}, /* AD7124_Gain_6 */
	{0x38, 0x500000, 3, 1}, /* AD7124_Gain_7 */
};

struct ad7124_init_param cn0391_ad7124_ip = {
	.spi_init = &cn0391_spi_ip,
	.regs = cn0391_ad7124_regs,
	.spi_rdy_poll_cnt = CN0391_SPI_RDY_POLL_CNT(SPI_BAUDRATE),
	.active_device = ID_AD7124_8,
	.ref_en = true,
	.power_mode = AD7124_HIGH_POWER,
	.mode = AD7124_IDLE,
	.setups = {
		/* Thermocouple: Gain=32, bipolar, internal ref, buffers */
		[CN0391_SETUP_TC_IDX] = {
			.bi_unipolar = true,
			.ref_buff = true,
			.ain_buff = true,
			.ref_source = INTERNAL_REF,
			.pga = AD7124_PGA_32,
		},
		/* Reference R5 and CJC RTD: Gain=1, bipolar, internal ref, buffers */
		[CN0391_SETUP_RTD_IDX] = {
			.bi_unipolar = true,
			.ref_buff = true,
			.ain_buff = true,
			.ref_source = INTERNAL_REF,
			.pga = AD7124_PGA_1,
		},
	},
	.chan_map = {
		/* Channel 0: Thermocouple - AIN0(+) / AIN15(-) */
		[0] = {
			.channel_enable = true,
			.setup_sel = CN0391_SETUP_TC_IDX,
			.ain = {
				.ainp = AD7124_AIN0,
				.ainm = AD7124_AIN15,
			},
		},
		/* Channel 1: Reference R5 - AIN8(+) / AIN15(-) */
		[1] = {
			.channel_enable = true,
			.setup_sel = CN0391_SETUP_RTD_IDX,
			.ain = {
				.ainp = AD7124_AIN8,
				.ainm = AD7124_AIN15,
			},
		},
		/* Channel 2: CJC RTD R1 - AIN1(+) / AIN8(-) */
		[2] = {
			.channel_enable = true,
			.setup_sel = CN0391_SETUP_RTD_IDX,
			.ain = {
				.ainp = AD7124_AIN1,
				.ainm = AD7124_AIN8,
			},
		},
		/* Channel 3: Thermocouple CH2 - AIN2(+) / AIN15(-) */
		[3] = {
			.channel_enable = true,
			.setup_sel = CN0391_SETUP_TC_IDX,
			.ain = {
				.ainp = AD7124_AIN2,
				.ainm = AD7124_AIN15,
			},
		},
		/* Channel 4: Reference R5 CH2 - AIN8(+) / AIN15(-) */
		[4] = {
			.channel_enable = true,
			.setup_sel = CN0391_SETUP_RTD_IDX,
			.ain = {
				.ainp = AD7124_AIN8,
				.ainm = AD7124_AIN15,
			},
		},
		/* Channel 5: CJC RTD R2 CH2 - AIN3(+) / AIN8(-) */
		[5] = {
			.channel_enable = true,
			.setup_sel = CN0391_SETUP_RTD_IDX,
			.ain = {
				.ainp = AD7124_AIN3,
				.ainm = AD7124_AIN8,
			},
		},
		/* Channel 6: Thermocouple CH1 - AIN4(+) / AIN15(-) */
		[6] = {
			.channel_enable = true,
			.setup_sel = CN0391_SETUP_TC_IDX,
			.ain = {
				.ainp = AD7124_AIN4,
				.ainm = AD7124_AIN15,
			},
		},
		/* Channel 7: Reference R5 CH1 - AIN8(+) / AIN15(-) */
		[7] = {
			.channel_enable = true,
			.setup_sel = CN0391_SETUP_RTD_IDX,
			.ain = {
				.ainp = AD7124_AIN8,
				.ainm = AD7124_AIN15,
			},
		},
		/* Channel 8: CJC RTD R3 CH1 - AIN5(+) / AIN8(-) */
		[8] = {
			.channel_enable = true,
			.setup_sel = CN0391_SETUP_RTD_IDX,
			.ain = {
				.ainp = AD7124_AIN5,
				.ainm = AD7124_AIN8,
			},
		},
		/* Channel 9: Thermocouple CH0 - AIN6(+) / AIN15(-) */
		[9] = {
			.channel_enable = true,
			.setup_sel = CN0391_SETUP_TC_IDX,
			.ain = {
				.ainp = AD7124_AIN6,
				.ainm = AD7124_AIN15,
			},
		},
		/* Channel 10: Reference R5 CH0 - AIN8(+) / AIN15(-) */
		[10] = {
			.channel_enable = true,
			.setup_sel = CN0391_SETUP_RTD_IDX,
			.ain = {
				.ainp = AD7124_AIN8,
				.ainm = AD7124_AIN15,
			},
		},
		/* Channel 11: CJC RTD R4 CH0 - AIN7(+) / AIN8(-) */
		[11] = {
			.channel_enable = true,
			.setup_sel = CN0391_SETUP_RTD_IDX,
			.ain = {
				.ainp = AD7124_AIN7,
				.ainm = AD7124_AIN8,
			},
		},
	},
};

struct cn0391_init_param cn0391_ip = {
	.ad7124_init = &cn0391_ad7124_ip,
};
