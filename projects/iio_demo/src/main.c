/***************************************************************************//**
 *   @file   iio_demo/src/main.c
 *   @brief  Implementation of Main Function.
 *   @author Cristian Pop (cristian.pop@analog.com)
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

#include "app_config.h"
#include "parameters.h"
#include "error.h"
#include "iio.h"
#include "iio_demo.h"
#include "irq.h"
#include "irq_extra.h"
#include "uart.h"
#include "uart_extra.h"

#ifdef XILINX_PLATFORM
#include <xparameters.h>
#include <xil_cache.h>
#include <xil_cache.h>
#endif // XILINX_PLATFORM

#ifdef ADUCM_PLATFORM

#include <sys/platform.h>
#include "adi_initialize.h"
#include <drivers/pwr/adi_pwr.h>

#define MAX_SIZE_BASE_ADDR		3000

static uint8_t in_buff[MAX_SIZE_BASE_ADDR];
static uint8_t out_buff[MAX_SIZE_BASE_ADDR];

#define DAC_DDR_BASEADDR	((uint32_t)out_buff)
#define ADC_DDR_BASEADDR	((uint32_t)in_buff)

#endif

int32_t platform_init()
{
#ifdef ADUCM_PLATFORM
	if (ADI_PWR_SUCCESS != adi_pwr_Init())
		return FAILURE;

	if (ADI_PWR_SUCCESS != adi_pwr_SetClockDivider(ADI_CLOCK_HCLK, 1u))
		return FAILURE;

	if (ADI_PWR_SUCCESS != adi_pwr_SetClockDivider(ADI_CLOCK_PCLK, 1u))
		return FAILURE;
	adi_initComponents();
#endif
	return SUCCESS;
}

/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main(void)
{
	int32_t status;
	char demo_device_output[] = "demo_device_output";
	char demo_device_input[] = "demo_device_input";

	/* iio demo configurations. */
	struct iio_demo_init_param iio_demo_in_init_par;

	/* iio demo configurations. */
	struct iio_demo_init_param iio_demo_out_init_par;

	/* iio descriptor. */
	struct iio_desc  *iio_desc;

	/* iio instance descriptor. */
	struct iio_demo_desc *iio_demo_in_desc;

	/* iio instance descriptor. */
	struct iio_demo_desc *iio_demo_out_desc;

	/* iio init param */
	struct iio_init_param iio_init_param;

	/* Initialization for UART. */
	struct uart_init_param uart_init_par;

	/* IRQ initial configuration. */
	struct irq_init_param irq_init_param;

	/* IRQ instance. */
	struct irq_ctrl_desc *irq_desc;


	status = platform_init();
	if (IS_ERR_VALUE(status))
		return status;

#ifdef XILINX_PLATFORM
	/* Xilinx platform dependent initialization for IRQ. */
	struct xil_irq_init_param platfomr_irq_init_par;

	platfomr_irq_init_par = (struct xil_irq_init_param ) {
#ifdef XPAR_INTC_SINGLE_DEVICE_ID
		.type = IRQ_PL,
#else
		.type = IRQ_PS,
#endif
	};
#endif // XILINX_PLATFORM

#ifdef ADUCM_PLATFORM
	/* Dummy value for Aducm platform dependent initialization for IRQ. */
	int32_t platfomr_irq_init_par = 0;
#endif //ADUCM_PLATFORM

	irq_init_param = (struct irq_init_param ) {
		.irq_ctrl_id = INTC_DEVICE_ID,
		.extra = &platfomr_irq_init_par
	};

	status = irq_ctrl_init(&irq_desc, &irq_init_param);
	if(status < 0)
		return status;

#ifdef XILINX_PLATFORM
	/* Xilinx platform dependent initialization for UART. */
	struct xil_uart_init_param platform_uart_init_par;

	platform_uart_init_par = (struct xil_uart_init_param) {
#ifdef XPAR_XUARTLITE_NUM_INSTANCES
		.type = UART_PL,
#else
		.type = UART_PS,
#endif
		.irq_id = UART_IRQ_ID,
		.irq_desc = irq_desc,
	};
#endif // XILINX_PLATFORM

#ifdef ADUCM_PLATFORM

	/* Aducm platform dependent initialization for UART. */
	struct aducm_uart_init_param platform_uart_init_par = {
		.parity = UART_NO_PARITY,
		.stop_bits = UART_ONE_STOPBIT,
		.word_length = UART_WORDLEN_8BITS
	};
#endif // ADUCM_PLATFORM
	uart_init_par = (struct uart_init_param) {
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE,
		.extra = &platform_uart_init_par
	};

	status = irq_global_enable(irq_desc);
	if (status < 0)
		return status;

	iio_init_param.phy_type = USE_UART;
	iio_init_param.phy_init_param = &uart_init_par;

	status = iio_init(&iio_desc, &iio_init_param);
	if(status < 0)
		return status;

	iio_demo_out_init_par = (struct iio_demo_init_param) {
		.name = demo_device_output,
		.num_channels = 4,
		.ddr_base_addr = DAC_DDR_BASEADDR,
		.ddr_base_size = MAX_SIZE_BASE_ADDR
	};

	status = iio_demo_init(&iio_demo_out_desc, &iio_demo_out_init_par);
	if (status < 0)
		return status;

	iio_demo_in_init_par = (struct iio_demo_init_param) {
		.name = demo_device_input,
		.num_channels = 4,
		.ddr_base_addr = DAC_DDR_BASEADDR,
		.ddr_base_size = MAX_SIZE_BASE_ADDR
	};

	status = iio_demo_init(&iio_demo_in_desc, &iio_demo_in_init_par);
	if (status < 0)
		return status;

	do {
		status = iio_step(iio_desc);
	} while (!IS_ERR_VALUE(status));

	return status;
}
