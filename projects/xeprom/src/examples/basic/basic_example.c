/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  BASIC example header for 24xx32a project
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include "24xx32a.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_error.h"
#include "no_os_eeprom.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Dummy example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/

struct data {
	/** Some data */
	uint32_t a;
	uint8_t b;
};

int basic_example_main()
{
	int ret = -22;
	struct no_os_eeprom_desc *eeprom;
	struct no_os_gpio_desc	*gpio_a0;
	struct data write_data = { 0 };

	write_data.a = 0x89ABCDEF;
	write_data.b = 0xDA;

	/* Clear the screen. */
	printf("%c",27);
	printf("%c",'[');
	printf("%c",'2');
	printf("%c",'J');

	no_os_mdelay(5);

	ret = no_os_gpio_get_optional(&gpio_a0, &gpio_a0_ip);
	if (ret)
		goto error;

	if (gpio_a0) {
		ret = no_os_gpio_direction_output(gpio_a0, NO_OS_GPIO_LOW);
		if (ret)
			goto error;
	}

	pr_info("\nBASIC EXAMPE\n");

	ret = no_os_eeprom_init(&eeprom, &eeprom_ip);
	if (ret)
		goto error;

	/* Write into EEPROM */
	ret =  no_os_eeprom_write(eeprom, 0x00, &write_data, sizeof(write_data));
	if (ret) {
		pr_info("ERROR writing data\n");
		goto error;
	}

	pr_info("SUCCES writing data.\n");

	uint8_t read_data[10] = { 0 };

	/* Write into EEPROM */
	ret =  no_os_eeprom_read(eeprom, 0x00, &read_data, sizeof(write_data));
	if (ret) {
		pr_info("ERROR reading data\n");
		goto error;
	}

	pr_info("Data read:\n");

	for (uint8_t i = 0; i < sizeof(write_data); i++)
		pr_info("%#x.\n", read_data[i]);

	while(1) {
		pr_info("*\n");

		no_os_mdelay(1000);
	}

error:
	pr_info("Error. Returned %d.\n", ret);
	// Allow time so that the message is displayed in the terminal
	no_os_mdelay(50);
	return ret;
}
