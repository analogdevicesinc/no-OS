/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for ltc4296 project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
#include "common_data.h"
#include "basic_example.h"
#include "no_os_delay.h"
#include "no_os_spi.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "no_os_pwm.h"
#include "ltc4296.h"

int basic_example_main()
{
	int ret;

	struct ltc4296_dev *ltc4296_desc;

	ret = ltc4296_init(&ltc4296_desc, &ltc4296_ip);
	if (ret) {
		pr_info("Initialization error!\n");
		return ret;
	}

	ret = ltc4296_port_prebias(ltc4296_desc, LTC_PORT0, LTC_CFG_APL_MODE);
	if (ret)
		goto err;

	ret = ltc4296_port_en(ltc4296_desc, LTC_PORT0);
	if (ret)
		goto err;

	no_os_mdelay(100);

	ltc4296_port_prebias(ltc4296_desc, LTC_PORT1, LTC_CFG_APL_MODE);
	if (ret)
		goto err;

	ltc4296_port_en(ltc4296_desc, LTC_PORT1);
	if (ret)
		goto err;

	no_os_mdelay(100);

	ret = ltc4296_port_prebias(ltc4296_desc, LTC_PORT2, LTC_CFG_APL_MODE);
	if (ret)
		goto err;

	ret = ltc4296_port_en(ltc4296_desc, LTC_PORT2);
	if (ret)
		goto err;

	no_os_mdelay(100);

	ret = ltc4296_port_prebias(ltc4296_desc, LTC_PORT3, LTC_CFG_APL_MODE);
	if (ret)
		goto err;

	ret = ltc4296_port_en(ltc4296_desc, LTC_PORT3);
	if (ret)
		goto err;

	no_os_mdelay(100);

	return 0;

err:
	ltc4296_remove(ltc4296_desc);

	return ret;
}
