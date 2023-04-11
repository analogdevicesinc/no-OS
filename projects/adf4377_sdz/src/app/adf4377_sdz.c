/***************************************************************************//**
 *   @file   projects/adf4377_sdz/src/app/adf4377_sdz.c
 *   @brief  Implementation of Main Function.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
#include <xparameters.h>
#include <xil_cache.h>
#include "no_os_spi.h"
#include "xilinx_spi.h"
#include "no_os_gpio.h"
#include "xilinx_gpio.h"
#include "no_os_error.h"
#include "adf4377.h"
#include "parameters.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

#ifdef IIO_SUPPORT
#include "iio_app.h"
#include "iio_adf4377.h"
#endif

int main(void)
{
	int32_t ret;
	uint8_t f_lock;
	struct adf4377_dev *dev;

	struct xil_spi_init_param xil_spi_init = {
		.flags = 0,
		.type = SPI_PS
	};

	struct xil_gpio_init_param xil_gpio_init = {
		.device_id = GPIO_DEVICE_ID,
		.type = GPIO_PS
	};

	struct no_os_gpio_init_param gpio_ce_param = {
		.number = GPIO_CE,
		.platform_ops = &xil_gpio_ops,
		.extra = &xil_gpio_init
	};

	struct no_os_gpio_init_param gpio_enclk1_param = {
		.number = GPIO_ENCLK1,
		.platform_ops = &xil_gpio_ops,
		.extra = &xil_gpio_init
	};

	struct no_os_gpio_init_param gpio_enclk2_param = {
		.number = GPIO_ENCLK2,
		.platform_ops = &xil_gpio_ops,
		.extra = &xil_gpio_init
	};

	struct no_os_spi_init_param spi_init = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 2000000,
		.chip_select = SPI_ADF4377_CS,
		.mode = NO_OS_SPI_MODE_0,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = &xil_spi_ops,
		.extra = &xil_spi_init
	};

	struct adf4377_init_param adf4377_param = {
		.dev_id = ADF4377,
		.spi_init = &spi_init,
		.gpio_ce_param = &gpio_ce_param,
		.gpio_enclk1_param = &gpio_enclk1_param,
		.gpio_enclk2_param = &gpio_enclk2_param,
		.spi4wire = true,
		.clkin_freq = 100000000,
		.cp_i = ADF4377_CP_10MA1,
		.muxout_select = ADF4377_MUXOUT_HIGH_Z,
		.ref_doubler_en = ADF4377_REF_DBLR_DIS,
		.f_clk = 1000000000,
		.clkout_op = ADF4377_CLKOUT_420MV
	};

	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();

	ret = adf4377_init(&dev, &adf4377_param);
	if (ret != 0) {
		pr_err("ADF4377 Initialization failed!\n");
		return -1;
	}

	pr_info("ADF4377 Successfully initialized!");

	ret = adf4377_spi_read(dev, ADF4377_REG(0x49), &f_lock);
	if (ret != 0) {
		pr_err("SPI Read Failed!");
	}

	if(ADF4377_LOCKED(f_lock))
		pr_info("Output Frequency Locked!");
	else
		pr_err("Output Frequency not Locked!");

#ifdef IIO_SUPPORT
	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("adf4377_dev", dev, &adf4377_iio_descriptor,
			       NULL, NULL, NULL),
	};
	return iio_app_run(NULL, 0, devices, NO_OS_ARRAY_SIZE(devices));
#endif

	/* Disable the instruction cache. */
	Xil_ICacheDisable();
	/* Disable the data cache. */
	Xil_DCacheDisable();

	return adf4377_remove(dev);
}
