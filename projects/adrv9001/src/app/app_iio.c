/***************************************************************************//**
 *   @file   app_iio.c
 *   @brief  adrv9002 application level iio utilities.
 *   @author Darius Berghe (darius.berghe@analog.com)
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
#include "error.h"
#include "uart.h"
#include "uart_extra.h"
#include "iio.h"
#include "parameters.h"
#include "app_iio.h"
#include "irq.h"
#include "irq_extra.h"

struct iio_data_buffer g_read_buff1 = {
	.buff = (void *)ADC1_DDR_BASEADDR,
	.size = 0xFFFFFFFF,
};

struct iio_data_buffer g_read_buff2 = {
	.buff = (void *)ADC2_DDR_BASEADDR,
	.size = 0xFFFFFFFF,
};

/**
 * @brief Application IIO setup.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t iio_server_init(struct iio_axi_adc_init_param *adc1_init,
			struct iio_axi_adc_init_param *adc2_init,
			struct iio_axi_dac_init_param *dac1_init,
			struct iio_axi_dac_init_param *dac2_init)
{
	struct irq_ctrl_desc *irq_desc;
	struct xil_irq_init_param xil_irq_init_par = {
		.type = IRQ_PS,
	};
	struct irq_init_param irq_init_par = {
		.irq_ctrl_id = INTC_DEVICE_ID,
		.extra = &xil_irq_init_par
	};
	struct xil_uart_init_param xil_uart_init_par = {
		.type = UART_PS,
		.irq_id = UART_IRQ_ID,
	};
	struct uart_init_param uart_init_par = {
		.baud_rate = 921600,
		.device_id = UART_DEVICE_ID,
		.extra = &xil_uart_init_par,
	};

	struct iio_desc *iio_desc;
	struct iio_axi_adc_desc *iio_axi_adc_desc;
	struct iio_axi_dac_desc *iio_axi_dac_desc;
	struct iio_init_param iio_init_par;
	struct iio_device *iio_dev_desc;
	int32_t status;

	status = irq_ctrl_init(&irq_desc, &irq_init_par);
	if(status < 0)
		return status;
	xil_uart_init_par.irq_desc = irq_desc;

	status = irq_global_enable(irq_desc);
	if (status < 0)
		return status;

	iio_init_par.phy_type = USE_UART;
	iio_init_par.uart_init_param = &uart_init_par;
	status = iio_init(&iio_desc, &iio_init_par);
	if (status < 0)
		return status;

	status = iio_axi_adc_init(&iio_axi_adc_desc, adc1_init);
	if (adc1_init && status < 0)
		return status;
	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &iio_dev_desc);
	status = iio_register(iio_desc, iio_dev_desc, "axi_adc1",
			      iio_axi_adc_desc, &g_read_buff1, NULL);
	if (status < 0)
		return status;

	status = iio_axi_dac_init(&iio_axi_dac_desc, dac1_init);
	if (dac1_init && status < 0)
		return status;
	iio_axi_dac_get_dev_descriptor(iio_axi_dac_desc, &iio_dev_desc);
	status = iio_register(iio_desc, iio_dev_desc, "axi_dac1",
			      iio_axi_dac_desc, NULL, NULL);
	if (status < 0)
		return status;

	status = iio_axi_adc_init(&iio_axi_adc_desc, adc2_init);
	if (adc2_init && status < 0)
		return status;
	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &iio_dev_desc);
	status = iio_register(iio_desc, iio_dev_desc, "axi_adc2",
			      iio_axi_adc_desc, &g_read_buff2, NULL);
	if (status < 0)
		return status;

	status = iio_axi_dac_init(&iio_axi_dac_desc, dac2_init);
	if (dac2_init && status < 0)
		return status;
	iio_axi_dac_get_dev_descriptor(iio_axi_dac_desc, &iio_dev_desc);
	status = iio_register(iio_desc, iio_dev_desc, "axi_dac2",
			      iio_axi_dac_desc, NULL, NULL);
	if (status < 0)
		return status;

	do {
		status = iio_step(iio_desc);
		if (status < 0)
			return status;

	} while (true);

	return SUCCESS;
}
