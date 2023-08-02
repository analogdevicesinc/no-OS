/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example header for demo_esp project
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include "basic_example.h"
#include "common_data.h"
#include "mxc_delay.h"
#include "mxc_errors.h"
#include "adc.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_gpio.h"
#include "no_os_timer.h"
#include "no_os_irq.h"
#include "no_os_print_log.h"
#include "mqtt_client.h"
#include "tcp_socket.h"
#include "wifi.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/


int basic_example_main()
{
	struct wifi_desc	*wifi;
	struct mqtt_desc	*mqtt;
	int32_t			ret;

	static struct no_os_irq_ctrl_desc	*irq_ctrl;
	static struct no_os_spi_desc		*spi;
	static struct no_os_i2c_desc		*i2c;
	struct no_os_gpio_desc			*gpio;

	// struct max_gpio_init_param gpio_extra_ip = {
	// 	.vssel = MXC_GPIO_VSSEL_VDDIOH,
	// };

	// struct no_os_irq_init_param irq_ip = {
	// 	.irq_ctrl_id = 0,
	// 	.platform_ops = IRQ_OPS,
	// 	.extra = 0
	// };

	struct no_os_gpio_init_param gpio_wifi_rst_ip = {
		.port = GPIO_PORT,
		.number = GPIO_NR,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	};

	// struct max_spi_init_param adxl355_spi_extra_ip  = {
	// 	.num_slaves = 1,
	// 	.polarity = SPI_SS_POL_LOW,
	// 	.vssel = MXC_GPIO_VSSEL_VDDIOH,
	// };

	// struct no_os_spi_init_param spi_ip = {
	// 	.device_id = 1,
	// 	.max_speed_hz = 2000000,
	// 	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	// 	.mode = NO_OS_SPI_MODE_0,
	// 	.platform_ops = SPI_OPS,
	// 	.chip_select = 1,
	// 	.extra = &adxl355_spi_extra_ip,
	// };

	// struct max_i2c_init_param max_i2c_extra = {
	// 	.vssel = MXC_GPIO_VSSEL_VDDIOH,
	// };

	// struct no_os_i2c_init_param i2c_ip = {
	// 	.device_id = 1,
	// 	.max_speed_hz = 400000,
	// 	.slave_address = 0xFF,
	// 	.platform_ops = &max_i2c_ops,
	// 	.extra = NULL,
	// };

	// ret = no_os_spi_init(&spi, &spi_ip);
	// if (ret)
	// 	return ret;

	// ret = no_os_spi_write_and_read(spi, "adalm2000", 9);

	gpio_wifi_rst_ip.port = 1;
	gpio_wifi_rst_ip.number = 25;

	ret= no_os_gpio_get(&gpio, &gpio_wifi_rst_ip);
	if (ret)
		return ret;

	ret= no_os_gpio_direction_output(gpio, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	ret= no_os_gpio_set_value(gpio, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	gpio_wifi_rst_ip.port = 2;
	gpio_wifi_rst_ip.number = 17;

	ret= no_os_gpio_get(&gpio, &gpio_wifi_rst_ip);
	if (ret)
		return ret;

	ret= no_os_gpio_direction_output(gpio, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	ret= no_os_gpio_set_value(gpio, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	gpio_wifi_rst_ip.port = 1;
	gpio_wifi_rst_ip.number = 26;

	ret= no_os_gpio_get(&gpio, &gpio_wifi_rst_ip);
	if (ret)
		return ret;

	ret= no_os_gpio_direction_output(gpio, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	ret= no_os_gpio_set_value(gpio, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;
	gpio_wifi_rst_ip.port = 1;
	gpio_wifi_rst_ip.number = 28;

	ret= no_os_gpio_get(&gpio, &gpio_wifi_rst_ip);
	if (ret)
		return ret;

	ret= no_os_gpio_direction_output(gpio, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	ret= no_os_gpio_set_value(gpio, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	gpio_wifi_rst_ip.port = 1;
	gpio_wifi_rst_ip.number = 29;

	ret= no_os_gpio_get(&gpio, &gpio_wifi_rst_ip);
	if (ret)
		return ret;

	ret= no_os_gpio_direction_output(gpio, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	ret= no_os_gpio_set_value(gpio, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	gpio_wifi_rst_ip.port = 1;
	gpio_wifi_rst_ip.number = 23;

	ret= no_os_gpio_get(&gpio, &gpio_wifi_rst_ip);
	if (ret)
		return ret;

	ret= no_os_gpio_direction_output(gpio, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	ret= no_os_gpio_set_value(gpio, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;


	gpio_wifi_rst_ip.port = 1;
	gpio_wifi_rst_ip.number = 30;

	ret= no_os_gpio_get(&gpio, &gpio_wifi_rst_ip);
	if (ret)
		return ret;

	ret= no_os_gpio_direction_output(gpio, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	ret= no_os_gpio_set_value(gpio, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;


	gpio_wifi_rst_ip.port = 1;
	gpio_wifi_rst_ip.number = 31;

	ret= no_os_gpio_get(&gpio, &gpio_wifi_rst_ip);
	if (ret)
		return ret;

	ret= no_os_gpio_direction_output(gpio, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	ret= no_os_gpio_set_value(gpio, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

// 	mxc_adc_conversion_req_t adc_conv;
// 	mxc_adc_slot_req_t single_slot = { MXC_ADC_CH_3 };
// 	mxc_adc_req_t adc_cfg;
// 	unsigned int flags;

// 	adc_cfg.clock = MXC_ADC_HCLK;
// 	adc_cfg.clkdiv = MXC_ADC_CLKDIV_16;
// 	adc_cfg.cal = MXC_ADC_EN_CAL;
// 	adc_cfg.trackCount = 4;
// 	adc_cfg.idleCount = 17;
// 	adc_cfg.ref = MXC_ADC_REF_INT_2V048;

//     	/* Initialize ADC */
//     	if (MXC_ADC_Init(&adc_cfg) != E_NO_ERROR) {
//         	printf("Error Bad Parameter\n");
//         while (1) {}
//     	}

// 	adc_conv.mode = MXC_ADC_ATOMIC_CONV;
//     	adc_conv.trig = MXC_ADC_TRIG_SOFTWARE;
//     //adc_conv.trig = MXC_ADC_TRIG_HARDWARE;
//     //adc_conv.hwTrig = MXC_ADC_TRIG_SEL_TEMP_SENS;
// 	adc_conv.avg_number = MXC_ADC_AVG_16;
// 	adc_conv.fifo_format = MXC_ADC_DATA_STATUS;
// 	adc_conv.fifo_threshold = 0;
// 	adc_conv.fifo_threshold = MAX_ADC_FIFO_LEN >> 1;
// 	adc_conv.lpmode_divder = MXC_ADC_DIV_2_5K_50K_ENABLE;
// 	adc_conv.num_slots = 0;

// 	MXC_ADC_Configuration(&adc_conv);

// 	MXC_ADC_SlotConfiguration(&single_slot, 0);

// 	int adc_val[8];
// 	int adc_val1;
// 	uint32_t adc_index = 0;

//  	while (1) {
// 		/* Flash LED when starting ADC cycle */
// 		MXC_ADC_StartConversion();
// 		while (1) {
// 			flags = MXC_ADC_GetFlags();
// 			// MXC_ADC_EnableConversion();
// 			// MXC_ADC_GetData(&adc_val1);
// 			// MXC_ADC_DisableConversion();
// 			if (flags & MXC_F_ADC_INTFL_SEQ_DONE) {
// 				adc_index += MXC_ADC_GetData(&adc_val[adc_index]);
// 			//printf("ADC Count2 = %X\n", adc_index);
// 				break;
// 			}

// 			if (flags & MXC_F_ADC_INTFL_FIFO_LVL) {
// 				adc_index += MXC_ADC_GetData(&adc_val[adc_index]);
// 			//printf("ADC Count1 = %X\n", adc_index);
// 			}
//   		}
// 		MXC_ADC_DisableConversion();
// 		printf("ADC Count1 = %X\n", adc_index);
// 	}

	return ret;
}
