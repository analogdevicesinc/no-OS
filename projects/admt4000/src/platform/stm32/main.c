/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Mbed platform of admt4000 project
 *   @author Jose Ramon San Buenaventura (jose.sanbuenaventura@analog.com)
 * 			 Louijie Compo  (louijie.compo@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#include "parameters.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "admt4000.h"

extern int example_main();

int main()
{
	struct admt4000_dev *admt;
	struct no_os_uart_desc *uart_desc;
	struct no_os_spi_desc *spi_desc;
	int ret;
	uint8_t sw_cfg[1] = {0xff};
	
	spi_extra_ip.get_input_clock = HAL_RCC_GetPCLK1Freq;
	spi_sel_b_extra_ip.get_input_clock = HAL_RCC_GetPCLK1Freq;
#ifdef TMC
	spi_tmc_extra_ip.get_input_clock = HAL_RCC_GetPCLK1Freq;
#endif

	stm32_init();

	/** Initialize UART interface */
	ret = no_os_uart_init(&uart_desc, &admt4000_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	/* Initialize SPI for switch configuration */
	ret = no_os_spi_init(&spi_desc, &spi_sel_b_spi_ip);
	if (ret)
		return ret;

	/* Configure ADG714 over SPI using SPI_SEL_B_N pin */
	ret = no_os_spi_write_and_read(spi_desc, sw_cfg, 1);
	if (ret)
		return ret;

	ret = example_main();

	if (ret)
		return ret;
}
