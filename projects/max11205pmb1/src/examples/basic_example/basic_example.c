/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of basic example for max11205pmb1 project.
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
#include "max11205.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "no_os_delay.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief basic example main execution.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/
int basic_example_main()
{
	int ret;
	struct max11205_dev *max11205_desc;
	struct no_os_irq_ctrl_desc *max11205_gpio_irq_desc;
	int16_t adc_data_raw;
	int32_t adc_data_mv;
	bool new_data_avail;

	/* Initialize GPIO IRQ controller */
	ret = no_os_irq_ctrl_init(&max11205_gpio_irq_desc, &max11205_gpio_irq_ip);
	if (ret)
		return ret;

	/* Initialize device */
	max11205_ip.irq_ctrl = max11205_gpio_irq_desc;
	ret = max11205_init(&max11205_desc, max11205_ip);
	if (ret)
		return ret;

	/* Continuously read data */
	while (1) {
		ret = max11205_get_data_raw(max11205_desc, &new_data_avail, &adc_data_raw);
		if (ret)
			return ret;
		/* Print data only if new data is available */
		if (new_data_avail) {
			pr_info("ADC raw data %d:\n", adc_data_raw);

			ret  = max11205_get_data_mv(max11205_desc, adc_data_raw, &adc_data_mv);
			if (ret)
				return ret;

			pr_info("ADC converted data %d [mV]:\n", adc_data_mv);
		}
	}

	return 0;
}
