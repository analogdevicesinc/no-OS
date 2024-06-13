/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Source file for basic example.
 *   @author Radu Sabau (radu.sabau@analog.com)
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
#include "basic_example.h"
#include "common_data.h"
#include "ad4692.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_error.h"

int basic_example_main()
{
	struct ad4692_desc *ad4692_desc;
	uint32_t val;
	int ret;

	ret = ad4692_init(&ad4692_desc, &ad4692_ip);
	if (ret)
		goto exit;

	if (ad4692_desc->mode != AD4692_MANUAL_MODE) {
		/* Activating channels 1 and 0 of the ADC in the sequencer. */
		ret = ad4692_std_seq_ch(ad4692_desc, NO_OS_GENMASK(1, 0), true, 0);
		if (ret)
			goto remove_ad4692;

		ret = ad4692_set_osc(ad4692_desc, AD4692_OSC_500KHZ);
		if (ret)
			goto remove_ad4692;

		ret = ad4692_gpio_set(ad4692_desc, AD4692_GPIO0,
				      AD4692_GPIO_OUTPUT_DATA_READYb);
		if (ret)
			goto remove_ad4692;
	}

	switch (ad4692_desc->mode) {
	case AD4692_CNV_CLOCK:
	case AD4692_CNV_BURST:
		ret = ad4692_sampling_enable(ad4692_desc, true);
		if (ret)
			goto remove_ad4692;

		/* ADC conversion time = (no. of channels + 1) * OSC_PERIOD */
		no_os_udelay(6);

		ret = ad4692_sampling_enable(ad4692_desc, false);
		if (ret)
			goto remove_ad4692;

		break;
	case AD4692_AUTONOMOUS:
		ret = ad4692_reg_write(ad4692_desc, AD4692_CONV_START_REG, 0x01);
		if (ret)
			goto remove_ad4692;

		/* ADC conversion time = (no. of channels + 1) * OSC_PERIOD */
		no_os_udelay(6);

		ret = ad4692_reg_write(ad4692_desc, AD4692_CONV_START_REG, 0x00);
		if (ret)
			goto remove_ad4692;

		break;
	case AD4692_SPI_BURST:
		ret = ad4692_reg_write(ad4692_desc, AD4692_CONV_START_REG, 0x01);
		if (ret)
			goto remove_ad4692;

		/* ADC conversion time = (no. of channels + 1) * OSC_PERIOD */
		no_os_udelay(6);
		break;
	case AD4692_MANUAL_MODE:
		break;
	default:
		return -EINVAL;
	}

	ret = ad4692_get_ch(ad4692_desc, 0, &val);
	if (ret)
		goto remove_ad4692;

	pr_info("Ch0 value : %x\n", val);

	ret = ad4692_get_ch(ad4692_desc, 1, &val);
	if (ret)
		goto remove_ad4692;

	pr_info("Ch1 value  : %x\n", val);

remove_ad4692:
	ad4692_remove(ad4692_desc);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}
