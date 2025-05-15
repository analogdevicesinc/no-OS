/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Mbed platform of admt4000 project
 *   @author Jose Ramon San Buenaventura (jose.sanbuenaventura@analog.com)
 * 			 Louijie Compo  (louijie.compo@analog.com)
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
#include "platform_includes.h"
#include "parameters.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_error.h"
#include "admt4000.h"

#if (ADMT4000_BASIC)
#include "admt4000_basic.h"
#endif

#if (ADMT4000_DIAG)
#include "admt4000_diag_example.h"
#endif

#if (ADMT4000_ECC)
#include "admt4000_ecc.h"
#endif

#if (ADMT4000_TEST)
#include "admt4000_function_test.h"
#endif

#if (ADMT4000_IIO)
#include "iio_example.h"
#include "iio_app.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief App main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the chosen example and will not return.
*******************************************************************************/
int main()
{
	struct admt4000_dev *admt;
	struct no_os_uart_desc *uart_desc;
	struct no_os_spi_desc *spi_desc;
	int ret;
	uint8_t sw_cfg[1] = {0xff};

#if (ADMT4000_BASIC + ADMT4000_DIAG + ADMT4000_TEST + ADMT4000_ECC + ADMT4000_IIO > 1)
#error Please enable only 1 example at a time.
#elif (ADMT4000_BASIC + ADMT4000_DIAG + ADMT4000_TEST + ADMT4000_ECC + ADMT4000_IIO == 0)
#error Please enable 1 example from the Makefile.
#endif

	/* Initialize SPI for switch configuration */
	ret = no_os_spi_init(&spi_desc, &spi_sel_b_spi_ip);
	if (ret)
		return ret;

	/* Configure ADG714 over SPI using SPI_SEL_B_N pin */
	ret = no_os_spi_write_and_read(spi_desc, sw_cfg, 1);
	if (ret)
		return ret;

#if (ADMT4000_IIO)
	ret = iio_example_main();
	if (ret)
		goto error;
#endif

	no_os_mdelay(100);

	/* Initialize UART */
	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	pr_info("\n SPI switch configured\n");

	/* Initialize ADMT */
	ret = admt4000_init(&admt, admt_ip);
	if (ret)
		return ret;

#if (ADMT4000_BASIC)
	ret = basic_meas(admt);

#elif (ADMT4000_DIAG)
	ret = diag_meas(admt);

#elif (ADMT4000_ECC)
	ret = ecc_main(admt);

#elif (ADMT4000_TEST)
	ret = function_test(admt);
#endif

error:
#ifndef ADMT4000_IIO
	no_os_uart_remove(uart_desc);
#endif
	admt4000_remove(admt);

	return ret;
}
