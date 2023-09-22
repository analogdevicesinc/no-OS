/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example header for ad7091r8-sdz project
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
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
#include "basic_example.h"
#include "common_data.h"
#include "ad7091r8.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

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
	struct ad7091r8_dev *ad7091r8_dev;
	uint16_t val;
	int ret;

	printf("AD7091R-8 basic_example - Read each channel sequentially\n\r");
	/* Use ad7091r4_ip and ad7091r2_ip for AD7091R-4 and AD7091R-2 respectively */
	ret = ad7091r8_init(&ad7091r8_dev, &ad7091r8_ip);
	if (ret)
		return ret;

	/* Enable all channels */
	ret = ad7091r8_spi_reg_write(ad7091r8_dev, 0x01, 0xFF);
	if (ret)
		goto error;

	while(1) {
		ret = ad7091r8_sequenced_read(ad7091r8_dev, &val);
		if (ret)
			goto error;

		printf("Channel %d raw ADC output: %d\n\r",
		       no_os_field_get(AD7091R8_REG_RESULT_CH_ID_MASK, val),
		       no_os_field_get(AD7091R8_REG_RESULT_DATA_MASK, val));
		no_os_mdelay(1000);
	}

error:
	printf("Error on AD7091R-8 basic_example: %d\n\r", ret);
	ad7091r8_remove(ad7091r8_dev);
	return ret;
}
