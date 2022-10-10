/***************************************************************************//**
* @file cn0561.c
* @brief Implementation of Main Function.
* @authors Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
* Copyright 2022(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in
* the documentation and/or other materials provided with the
* distribution.
* - Neither the name of Analog Devices, Inc. nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
* - The use of this software may or may not infringe the patent rights
* of one or more patent holders. This license does not release you
* from the requirement that you obtain separate licenses from these
* patent holders to use this software.
* - Use of the software either in source or binary form, must be run
* on or directly connected to an Analog Devices Inc. component.
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
#include <stdio.h>
#include <sleep.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <xil_cache.h>
#include <xparameters.h>
#include <math.h>
#include "xil_printf.h"
#include "spi_engine.h"
#include "ad713x.h"
#include "no_os_spi.h"
#include "spi_extra.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "gpio_extra.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "parameters.h"
#include "no_os_pwm.h"
#include "axi_pwm_extra.h"
#include "clk_axi_clkgen.h"
#include "axi_dmac.h"

#define CN0561_ZED_CARRIER

#ifdef IIO_SUPPORT
#include "no_os_irq.h"
#include "irq_extra.h"
#include "no_os_uart.h"
#include "uart_extra.h"
#include "iio_ad713x.h"
#include "iio.h"
#include "iio_app.h"
#endif // IIO_SUPPORT

// GPIO SUPPORT TESTING
#include "xgpiops.h"
#include "xstatus.h"
#include "xplatform_info.h"

static uint32_t adc_buffer[ADC_BUFFER_SIZE] __attribute__((aligned));

int main()
{
	struct axi_clkgen *clkgen_cn0561;
	struct axi_clkgen_init clkgen_cn0561_init = {
		.base = XPAR_AXI_CN0561_CLKGEN_BASEADDR,
		.name = "cn0561_clkgen",
		.parent_rate = 100000000
	};
	struct ad713x_dev *cn0561_dev;
	struct ad713x_init_param cn0561_init_param;
	uint32_t i = 0, j;
	int32_t ret;
	const float lsb = 4.096 / (pow(2, 23));
	float data;
	uint32_t spi_eng_dma_flg = DMA_LAST | DMA_PARTIAL_REPORTING_EN;
	struct spi_engine_offload_init_param spi_engine_offload_init_param;
	struct spi_engine_offload_message spi_engine_offload_message;
	uint32_t spi_eng_msg_cmds[1];
	static struct xil_spi_init_param spi_engine_init_params = {
		.type = SPI_PS,
	};
	struct xil_gpio_init_param gpio_extra_param;
	struct no_os_gpio_init_param cn0561_pnd = {
		.number = GPIO_PDN,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
#ifdef CN0561_ZED_CARRIER
	struct no_os_gpio_init_param cn0561_mode = {
		.number = GPIO_MODE,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
	struct no_os_gpio_init_param cn0561_resetn = {
		.number = GPIO_RESETN,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
#endif
	struct no_os_spi_desc *spi_eng_desc;
	struct spi_engine_init_param spi_eng_init_param  = {
		.type = SPI_ENGINE,
		.spi_engine_baseaddr = CN0561_SPI_ENGINE_BASEADDR,
		.cs_delay = 0,
		.data_width = 32, // HERE MODIFY SPI FRAME SIZE
		.ref_clk_hz = CN0561_SPI_ENG_REF_CLK_FREQ_HZ
	};
	const struct no_os_spi_init_param spi_eng_init_prm  = {
		.chip_select = CN0561_SPI_CS,
		.max_speed_hz = 48000000,
		.mode = NO_OS_SPI_MODE_1,
		.platform_ops = &spi_eng_platform_ops,
		.extra = (void*)&spi_eng_init_param,
	};

	struct no_os_pwm_desc *axi_pwm;
	struct axi_pwm_init_param axi_zed_pwm_init0 = {
		.base_addr = XPAR_ODR_GENERATOR_BASEADDR,
		.ref_clock_Hz = 96000000,
		.channel = 1
	};

	struct axi_pwm_init_param axi_zed_pwm_init1 = {
		.base_addr = XPAR_ODR_GENERATOR_BASEADDR,
		.ref_clock_Hz = 96000000,
		.channel = 2
	};

	struct no_os_pwm_init_param axi_pwm_init0 = {
		.period_ns = 850,
		.duty_cycle_ns = 130,
		.phase_ns = 0,
		.extra = &axi_zed_pwm_init0
	};

	struct no_os_pwm_init_param axi_pwm_init1 = {
		.period_ns = 850,
		.duty_cycle_ns = 1,
		.phase_ns = 30,
		.extra = &axi_zed_pwm_init1
	};

	gpio_extra_param.device_id = GPIO_DEVICE_ID;
	gpio_extra_param.type = GPIO_PS;

	cn0561_init_param.adc_data_len = ADC_24_BIT_DATA; // HERE MODIFY DATA SIZE ADC
	cn0561_init_param.clk_delay_en = false;
	cn0561_init_param.crc_header = CRC_6;  // HERE MODIFY/ACTIVATE THE CRC HEADER
//	cn0561_init_param.crc_header = NO_CRC; // HERE MODIFY/ACTIVATE THE CRC HEADER
	cn0561_init_param.dev_id = ID_AD4134;
	cn0561_init_param.format = QUAD_CH_PO;
	cn0561_init_param.gpio_dclkio = NULL;
	cn0561_init_param.gpio_dclkmode = NULL;
	cn0561_init_param.gpio_pnd = &cn0561_pnd;
#ifdef CN0561_ZED_CARRIER
	cn0561_init_param.gpio_mode = &cn0561_mode;
	cn0561_init_param.gpio_resetn = &cn0561_resetn;
#else
	cn0561_init_param.gpio_mode = NULL;
	cn0561_init_param.gpio_resetn = NULL;
#endif
	cn0561_init_param.mode_master_nslave = false;
	cn0561_init_param.dclkmode_free_ngated = false;
	cn0561_init_param.dclkio_out_nin = false;
	cn0561_init_param.pnd = true;
	cn0561_init_param.spi_init_prm.chip_select = CN0561_SPI_CS;
	cn0561_init_param.spi_init_prm.device_id = SPI_DEVICE_ID;
	cn0561_init_param.spi_init_prm.max_speed_hz = 1000000;
	cn0561_init_param.spi_init_prm.mode = NO_OS_SPI_MODE_0;
	cn0561_init_param.spi_init_prm.platform_ops = &xil_spi_ops;
	cn0561_init_param.spi_init_prm.extra = (void *)&spi_engine_init_params;
	cn0561_init_param.spi_common_dev = 0;

	spi_eng_msg_cmds[0] = READ(4);

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	ret = axi_clkgen_init(&clkgen_cn0561, &clkgen_cn0561_init);
	if (ret != 0)
		return -1;

	ret = axi_clkgen_set_rate(clkgen_cn0561, CN0561_SPI_ENG_REF_CLK_FREQ_HZ);
	if (ret != 0)
		return -1;

//	uint32_t phase_test; // Variable to test phase_shift

	ret = no_os_pwm_init(&axi_pwm, &axi_pwm_init0);
	if (ret != 0)
		return ret;

// TEST PHASE SHIFT
//	ret = no_os_pwm_set_phase(&axi_pwm, 0);
//	ret = no_os_pwm_get_phase(axi_pwm, &phase_test);

	ret = no_os_pwm_init(&axi_pwm, &axi_pwm_init1);
	if (ret != 0)
		return ret;

// TEST PHASE SHIFT
//	ret = no_os_pwm_set_phase(&axi_pwm, 0);
//	ret = no_os_pwm_get_phase(axi_pwm, &phase_test);

	ret = ad713x_init(&cn0561_dev, &cn0561_init_param);
	if (ret != 0)
		return -1;

	for(int adc_channel = CH0; adc_channel <= CH3; adc_channel++) {
		ret = ad713x_dig_filter_sel_ch(cn0561_dev, SINC3, adc_channel);
		if (ret != 0)
			return -1;
	}

/*
 * TESTING PIN CONFIGURATION ADC
 *
	u32 Data;
	XGpioPs Gpio;
	XGpioPs_Config *ConfigPtr;
	ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
	int Status = XGpioPs_CfgInitialize(&Gpio, ConfigPtr,
						ConfigPtr->BaseAddr);

	XGpioPs_SetDirectionPin(&Gpio, GPIO_PINBSPI, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, GPIO_PINBSPI, 1);

	XGpioPs_SetDirectionPin(&Gpio, GPIO_6, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, GPIO_6, 1);
	XGpioPs_SetDirectionPin(&Gpio, GPIO_7, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, GPIO_7, 1);

	XGpioPs_SetDirectionPin(&Gpio, GPIO_5, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, GPIO_5, 1);

	XGpioPs_SetDirectionPin(&Gpio, GPIO_4, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, GPIO_4, 1);


	XGpioPs_WritePin(&Gpio, GPIO_PINBSPI, 0x0);
	XGpioPs_WritePin(&Gpio, GPIO_4, 0x0);
	XGpioPs_WritePin(&Gpio, GPIO_5, 0x0);
	XGpioPs_WritePin(&Gpio, GPIO_6, 0x0);
	XGpioPs_WritePin(&Gpio, GPIO_7, 0x0);

	Data = XGpioPs_ReadPin(&Gpio, GPIO_PINBSPI);
	Data = XGpioPs_ReadPin(&Gpio, GPIO_6);
	Data = XGpioPs_ReadPin(&Gpio, GPIO_7);
	Data = XGpioPs_ReadPin(&Gpio, GPIO_5);
	Data = XGpioPs_ReadPin(&Gpio, GPIO_4);
 *
*/
	no_os_mdelay(1000);

	ret = ad713x_spi_reg_write(cn0561_dev, AD713X_REG_GPIO_DIR_CTRL, 0xE7);
	if (ret != 0)
		return -1;
	ret = ad713x_spi_reg_write(cn0561_dev, AD713X_REG_GPIO_DATA, 0x84);
	if (ret != 0)
		return -1;

	spi_engine_offload_init_param.rx_dma_baseaddr = CN0561_DMA_BASEADDR;
	spi_engine_offload_init_param.offload_config = OFFLOAD_RX_EN;
	spi_engine_offload_init_param.dma_flags = &spi_eng_dma_flg;

	ret = no_os_spi_init(&spi_eng_desc, &spi_eng_init_prm);
	if (ret != 0)
		return -1;

	ret = spi_engine_offload_init(spi_eng_desc, &spi_engine_offload_init_param);
	if (ret != 0)
		return -1;

	spi_engine_offload_message.commands = spi_eng_msg_cmds;
	spi_engine_offload_message.no_commands = NO_OS_ARRAY_SIZE(spi_eng_msg_cmds);
	spi_engine_offload_message.commands_data = NULL;
	spi_engine_offload_message.rx_addr = (uint32_t)adc_buffer;
	spi_engine_offload_message.tx_addr = 0xA000000;

#ifdef IIO_SUPPORT
	struct iio_data_buffer rd_buff = {
		.buff = (void *)adc_buffer,
		.size = ADC_BUFFER_SIZE
	};
	struct ad713x_iio *iio_desc;
	struct ad713x_iio_init_param iio_desc_param = {
		.drv_dev = cn0561_dev,
		.vref_int = 4,
		.vref_micro = 96000,
		.spi_eng_desc = spi_eng_desc,
		.dcache_invalidate_range =
		(void (*)(uint32_t,  uint32_t))Xil_DCacheInvalidateRange,
		.iio_dev = &ad713x_iio_desc
	};

	ret = iio_ad713x_init(&iio_desc, &iio_desc_param);
	if (ret < 0)
		return ret;

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("ad4134", iio_desc, &ad713x_iio_desc,
			       &rd_buff, NULL),
	};

	return iio_app_run(devices, NO_OS_ARRAY_SIZE(devices));

#endif /* IIO_SUPPORT */

/*
 *  TEST READING OFFSET REGs
*/

/*
#define REG_PULSE_0_OFFSET 0x48
#define REG_PULSE_1_OFFSET 0x54
#define REG_PULSE_2_OFFSET 0x60

u8 buf_gpio6;
u8 buf_gpio7;
u32 buf_gpio_id;
u8 buf;

	//buf = Xil_In32(XPAR_AXI_PWM_GEN_0_BASEADDR);
	//buf = Xil_In32(XPAR_AXI_PWM_GEN_0_BASEADDR + REG_PULSE_1_OFFSET);
	//buf = Xil_In32(XPAR_AXI_PWM_GEN_0_BASEADDR + REG_PULSE_2_OFFSET);
	//Xil_Out32(XPAR_AXI_PWM_GEN_0_BASEADDR + REG_PULSE_0_OFFSET, 0);
	//buf = Xil_In32(XPAR_AXI_PWM_GEN_0_BASEADDR + REG_PULSE_0_OFFSET);

/*
 *  TEST READING GPIOs Xil

 *  All gpio reads show 1, if not set prior to reading.
 *  Writing and reading the gpios all seem to work properly.

	buf_gpio_id = Xil_In32(XPAR_PS7_GPIO_0_BASEADDR + GPIO_DEVICE_ID);
	buf = Xil_In8(XPAR_PS7_GPIO_0_BASEADDR + GPIO_0);
*/
	ret = spi_engine_offload_transfer(spi_eng_desc, spi_engine_offload_message,
					  (CN0561_FMC_CH_NO * CN0561_FMC_SAMPLE_NO));
	if (ret != 0)
		return ret;

	Xil_DCacheInvalidateRange((INTPTR)adc_buffer,
				  CN0561_FMC_SAMPLE_NO * CN0561_FMC_CH_NO *
				  sizeof(uint32_t));

	for(i = 0; i < CN0561_FMC_SAMPLE_NO; i++) {
		j = 0;
		printf("%lu: ", i);
		while(j < CN0561_FMC_CH_NO) {
			adc_buffer[CN0561_FMC_CH_NO*i+j] &= 0xffffff00;
			adc_buffer[CN0561_FMC_CH_NO*i+j] >>= 8;
			data = lsb * (int32_t)adc_buffer[CN0561_FMC_CH_NO*i+j];
			if(data > 4.095)
				data = data - 8.192;
			if(j == 0 || j==3) // READ CH0 CH3 ONLY (those were used on board)
			printf("CH%lu: 0x%08lx = %+1.5fV ", j,
			       adc_buffer[CN0561_FMC_CH_NO*i+j], data);
			if(j == (CN0561_FMC_CH_NO - 1))
				printf("\n");
			j++;
		}
	}

	/*
	 *  READ ADC REGISTER VALUES
	*/
	uint8_t reg_data;
	uint8_t reg_addr;
	for(reg_addr = 0x0; reg_addr <= 0x7; reg_addr++) {
		ret = ad713x_spi_reg_read(cn0561_dev, reg_addr, &reg_data);
			if (ret != 0)
				printf("REG%x: error read\n", reg_addr);
		printf("REG%x: 0x%08x\n", reg_addr, reg_data);
	}

	for(reg_addr = 0xA; reg_addr <= 0x42; reg_addr++) {
			ret = ad713x_spi_reg_read(cn0561_dev, reg_addr, &reg_data);
				if (ret != 0)
					printf("REG%x: error read\n", reg_addr);
			printf("REG%x: 0x%08x\n", reg_addr, reg_data);
		}

	for(reg_addr = 0x47; reg_addr <= 0x48; reg_addr++) {
			ret = ad713x_spi_reg_read(cn0561_dev, reg_addr, &reg_data);
				if (ret != 0)
					printf("REG%x: error read\n", reg_addr);
			printf("REG%x: 0x%08x\n", reg_addr, reg_data);
		}

	ad713x_remove(cn0561_dev);
	print("Bye\n\r");

	return 0;
}
