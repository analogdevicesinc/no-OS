/***************************************************************************//**
 *   @file   iio_aducm3029/src/main.c
 *   @brief  Implementation of Main Function.
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#include "parameters.h"
#include "platform_init.h"
#include "no_os_error.h"
#include "aducm3029_adc.h"
#include "aducm3029_pwm.h"
#include "no_os_uart.h"
#include "aducm3029_uart.h"
#include "no_os_pwm.h"
#include "no_os_util.h"
#include <stdio.h>
#include <string.h>

static uint16_t	adc_buffer[ADC_BUFF_SIZE];

#ifdef IIO_SUPPORT
#include "iio_aducm3029.h"
#include "iio_app.h"

static struct iio_data_buffer adc_read_buff = {
	.buff = adc_buffer,
	.size = ADC_BUFF_SIZE * sizeof(uint16_t)
};
#endif

static int32_t initialize_uart(struct no_os_uart_desc **uart)
{
	struct no_os_uart_init_param uart_init_par = {
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.size =  NO_OS_UART_CS_8,
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE,
		.platform_ops = &aducm_uart_ops,
	};

	return no_os_uart_init(uart, &uart_init_par);
}

static int32_t init_pwms(struct no_os_pwm_desc **pwms)
{
	uint32_t i;
	int32_t ret;
	uint32_t duty = 100000000;
	struct no_os_pwm_init_param		pwm_init_par = {
		.period_ns = 400000000,
		.polarity = NO_OS_PWM_POLARITY_HIGH,
		.platform_ops = &aducm3029_pwm_ops,
		.extra = NULL
	};
	struct no_os_pwm_desc *pwm;
	for (i = 0; i < 3; i++) {
		pwm_init_par.id = i;
		pwm_init_par.duty_cycle_ns = (i + 1) * duty;
		ret = no_os_pwm_init(&pwm, &pwm_init_par);
		if (NO_OS_IS_ERR_VALUE(ret))
			return -ret;
		pwms[i] = pwm;
		ret = no_os_pwm_enable(pwms[i]);
		if (NO_OS_IS_ERR_VALUE(ret))
			return -ret;
	}

	return 0;
}

int main(void)
{
	int32_t status;

	status = platform_init();
	if (NO_OS_IS_ERR_VALUE(status))
		return status;

#ifdef IIO_SUPPORT
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	struct no_os_uart_init_param uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.asynchronous_rx = true,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = NULL,
		.platform_ops = &aducm_uart_ops,
	};

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("adcum3029", &g_aducm3029_desc,
			       &iio_aducm3029_desc,
			       &adc_read_buff, NULL, NULL)
	};

	/**
	 * In order to read data adc_enable must be set to 1
	 * In order to start pwms en from each pwm must be set to 1
	 * This can be done from an iio client (Ex. IIO Oscilloscope)
	 * but, to just start reading demo data they will be enabled here.
	 */
	set_global_attr(&g_aducm3029_desc, "1", 1, NULL, ADC_ENABLE);
	struct iio_ch_info ch = {.ch_num = 6, .ch_out = 1};
	set_pwm_attr(&g_aducm3029_desc, "1", 1, &ch, PWM_ENABLE);
	ch.ch_num = 7;
	set_pwm_attr(&g_aducm3029_desc, "1", 1, &ch, PWM_ENABLE);
	ch.ch_num = 8;
	set_pwm_attr(&g_aducm3029_desc, "1", 1, &ch, PWM_ENABLE);

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = uart_ip;

	status = iio_app_init(&app, app_init_param);
	if (status)
		return status;

	return iio_app_run(app);
#endif

	struct adc_init_param	adc_init_param = {0};
	struct adc_desc		*adc;
	struct no_os_uart_desc	*uart;
	struct no_os_pwm_desc		*pwms[3];

	status = aducm3029_adc_init(&adc, &adc_init_param);
	if (NO_OS_IS_ERR_VALUE(status))
		return status;

	status = init_pwms(pwms);
	if (NO_OS_IS_ERR_VALUE(status))
		return status;

	status = initialize_uart(&uart);
	if (NO_OS_IS_ERR_VALUE(status))
		return status;

	/* Enabled ADC channels */
	uint32_t ch_mask =
		ADUCM3029_CH(0) |
		ADUCM3029_CH(1) |
		ADUCM3029_CH(2) |
		ADUCM3029_CH(3) |
		ADUCM3029_CH(4) |
		ADUCM3029_CH(5) |
		0;
	char buff[100];
	uint32_t active_ch = no_os_hweight32(ch_mask);
	uint32_t nb_samples = 10;
	uint32_t i,j,k;



	status = aducm3029_adc_update_active_channels(adc, ch_mask);
	if (NO_OS_IS_ERR_VALUE(status))
		return status;
	while (true) {
		status = aducm3029_adc_read(adc, adc_buffer, nb_samples);
		if (NO_OS_IS_ERR_VALUE(status))
			return status;
		k = 0;
		for (i = 0; i < nb_samples; i++) {
			uint32_t n = 0;
			for (j = 0; j < active_ch; j++) {
				n += sprintf(buff + n, "ch%d:%d",j,adc_buffer[k++]);
				if (j == active_ch - 1)
					n += sprintf(buff + n, "\r\n");
				else
					n += sprintf(buff + n, ",");
			}
			status = no_os_uart_write(uart, buff, strlen(buff));
			if (NO_OS_IS_ERR_VALUE(status))
				return status;
		}
	}
}
