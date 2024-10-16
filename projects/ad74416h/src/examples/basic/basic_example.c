/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example header for eval-ad74416h project
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
#include "ad74416h.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int basic_example_main()
{
	uint8_t command_0[10] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x02, 0x80, 0x00, 0x00, 0x82};
	struct ad74416h_desc *ad74416h_desc;
	uint32_t byte_count;
	uint32_t reg_read;
	int ret;
	int i;

	ret = ad74416h_init(&ad74416h_desc, &ad74416h_ip);
	if (ret)
		goto error;

	pr_info("AD74416H Successfully Initialized\r\n");

	ret = ad74416h_gpio_set(ad74416h_desc, AD74416H_CH_B, NO_OS_GPIO_HIGH);
	if (ret)
		goto error_ad74416h;

	ret = ad74416h_gpio_set(ad74416h_desc, AD74416H_CH_D, NO_OS_GPIO_HIGH);
	if (ret)
		goto error_ad74416h;


	/* Configure the channel in the appropriate function (current input loop powered with HART) */
	ret = ad74416h_reg_update(ad74416h_desc, AD74416H_CH_FUNC_SETUP(0),
				  AD74416H_CH_FUNC_SETUP_MSK,
				  AD74416H_CURRENT_IN_LOOP_HART);
	if (ret)
		goto error_ad74416h;

	/* Wait 200 μs before proceeding with another step */
	no_os_udelay(200);

	/* Wait until HART Compliant Slew is Settled */
	while (1) {
		ret = ad74416h_reg_read(ad74416h_desc,
					AD74416H_OUTPUT_CONFIG(0), &reg_read);
		if (ret)
			goto error_ad74416h;
		reg_read = no_os_field_get(AD74416H_HART_COMPL_SETTLED_MSK,
					   reg_read);
		if (reg_read == 1)
			break;
	}

	/* Enable the HART slew option SLEW_EN to binary 10 (SLEW_HART_COMPL) */
	ret = ad74416h_reg_update(ad74416h_desc, AD74416H_OUTPUT_CONFIG(0),
				  AD74416H_SLEW_EN_MSK, 2);
	if (ret)
		goto error_ad74416h;

	/* Power up the HART modem */
	ret = ad74416h_reg_update(ad74416h_desc, AD74416H_HART_CONFIG(0),
				  AD74416H_MODEM_PWRUP_MSK, 1);
	if (ret)
		goto error_ad74416h;

	/* Clear the Transmit FIFO. */
	ret = ad74416h_reg_update(ad74416h_desc, AD74416H_HART_FCR(0),
				  NO_OS_BIT(2), 1);
	if (ret)
		goto error_ad74416h;

	/* Clear the Receive FIFO. */
	ret = ad74416h_reg_update(ad74416h_desc, AD74416H_HART_FCR(0),
				  NO_OS_BIT(1), 1);
	if (ret)
		goto error_ad74416h;

	/* Load the HART transmit first in first out (FIFO) with data required
	   for transmission */
	pr_info("Request (TX FIFO Byte Count: %d):\r\n",
		sizeof(command_0)/sizeof(command_0[0]));
	for (i = 0; i < sizeof(command_0)/sizeof(command_0[0]); i++) {
		ret = ad74416h_reg_write(ad74416h_desc, AD74416H_HART_TX(0),
					 command_0[i]);
		if (ret)
			goto error_ad74416h;
		pr_info("%02X ", command_0[i]);
	}
	pr_info("\r\n");

	/* Ensure that the HART alerts are cleared */
	ret = ad74416h_reg_write(ad74416h_desc, AD74416H_HART_ALERT_STATUS(0),
				 0xffff);
	if (ret)
		goto error_ad74416h;

	/* Set the RTS bit (request to send) to start HART transmissions */
	ret = ad74416h_reg_write(ad74416h_desc, AD74416H_HART_MCR(0), 1);
	if (ret)
		goto error_ad74416h;

	/* The RTS bit will be automatically cleared once the message transmission is
	   completed unless the AUTO_CLR_RTS bit is cleared */
	while (1) {
		ret = ad74416h_reg_read(ad74416h_desc, AD74416H_HART_MCR(0),
					&reg_read);
		if (ret)
			return ret;
		if (reg_read == 0)
			break;
	}

	no_os_mdelay(500);

	/* Monitor the HART_ALERT_STATUSn register for status alerts on the
	   progress of the HART communication */
	ret = ad74416h_reg_read(ad74416h_desc, AD74416H_HART_ALERT_STATUS(0),
				&reg_read);
	if (ret)
		return ret;
	pr_info("HART_ALERT_STATUS: 0x%02X\r\n", reg_read);

	ret = ad74416h_reg_read(ad74416h_desc, AD74416H_HART_RFC(0),
				&byte_count);
	if (ret)
		return ret;
	pr_info("Response (RX FIFO Byte Count: %d):\r\n", byte_count);

	for (i = 0; i < byte_count; i++) {
		ret = ad74416h_reg_read(ad74416h_desc, AD74416H_HART_RX(0),
					&reg_read);
		if (ret)
			return ret;
		if (i == byte_count - 1) {
			pr_info("%02X\r\n", reg_read);
			break;
		}
		pr_info("%02X ", reg_read);
	}

	return 0;

error_ad74416h:
	ad74416h_remove(ad74416h_desc);
error:
	pr_info("Error!\r\n");

	return ret;
}
