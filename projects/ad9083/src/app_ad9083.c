/***************************************************************************//**
 *   @file   app_ad9083.c
 *   @brief  Implementation of app_ad9083.
 *   @author Cristian Pop (cristian.pop@analog.com)
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

#include <stdlib.h>
#include "inttypes.h"
#include "app_ad9083.h"
#include "parameters.h"
#include "spi.h"
#include "spi_extra.h"
#include "gpio_extra.h"
#include "error.h"
#include "util.h"
#include "delay.h"
#include "print_log.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Check sysref is submultiple of lmfc.
 * @param lmfc - Multiframe clock.
 * @param sysref - System reference clock.
 * @return true if is submultiple, false otherwise.
 */
bool app_ad9083_check_sysref_rate(uint32_t lmfc, uint32_t sysref)
{
	uint32_t div, mod;

	div = lmfc / sysref;
	mod = lmfc % sysref;

	/* Ignore minor deviations that can be introduced by rounding. */
	return mod <= div || mod >= sysref - div;
}

/**
 * @brief Check AD9083 link status.
 * @param app - AD9083 app descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t app_ad9083_status(struct app_ad9083 *app)
{
	uint16_t stat, retry = 3;
	int32_t ret;

	do {
		ret = adi_ad9083_jesd_tx_link_status_get(
			      &app->ad9083_phy->adi_ad9083, &stat);
		if (ret)
			return -EFAULT;


		if ((stat & 0xFF) == 0x7D)
			ret = SUCCESS;
		else
			ret = -EIO;

		if (ret == 0 || retry == 0)
			pr_info("JESD RX (JTX) , state_204b %x, SYNC %s, PLL %s, PHASE %s, MODE %s\n",
				stat & 0x0f,
				stat & BIT(4) ? "deasserted" : "asserted",
				stat & BIT(5) ? "locked" : "unlocked",
				stat & BIT(6) ? "established" : "lost",
				stat & BIT(7) ? "invalid" : "valid");
		else
			udelay(20000);

	} while (ret && retry--);

	return SUCCESS;
}

/**
 * @brief Initialize the AD9083 app.
 * @param app - AD9083 app descriptor.
 * @param init_param - The structure that contains the app initial
 * 		       parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t app_ad9083_init(struct app_ad9083 **app,
			struct app_ad9083_init *init_param)
{
	int32_t status;
	struct app_ad9083 *app_ad9083;
	struct xil_spi_init_param xil_spi_param = {
		.type = SPI_PS,
		.device_id = 0,
	};

	// clock chip spi settings
	struct spi_init_param ad9083_spi_init_param = {
		.max_speed_hz = 10000000,
		.mode = SPI_MODE_0,
		.chip_select = SPI_AD9083_CS,

		.platform_ops = &xil_platform_ops,
		.extra = &xil_spi_param
	};
	struct xil_gpio_init_param  xil_gpio_param = {
		.type = GPIO_PS,
		.device_id = GPIO_DEVICE_ID
	};
	struct gpio_init_param	gpio_phy_resetb = {
		.number = AD9803_RSTB,
		.platform_ops = &xil_gpio_platform_ops,
		.extra = &xil_gpio_param
	};
	struct gpio_init_param	gpio_phy_pd = {
		.number = AD9803_PWDN,
		.platform_ops = &xil_gpio_platform_ops,
		.extra = &xil_gpio_param
	};

	struct ad9083_init_param ad9083_init_param = {
		.spi_init = &ad9083_spi_init_param,
		.gpio_reset = &gpio_phy_resetb,
		.gpio_pd = &gpio_phy_pd,
		.uc = init_param->uc,
		.jesd_rx_clk = init_param->jesd_rx_clk,
	};

	app_ad9083 = (struct app_ad9083 *)calloc(1, sizeof(*app_ad9083));
	if (!app_ad9083)
		return FAILURE;

	status = ad9083_init(&app_ad9083->ad9083_phy, &ad9083_init_param);
	if (status != SUCCESS) {
		pr_err("error: %"PRId32" ad9083_initialize() \n", status);
		free(app_ad9083);

		return FAILURE;
	}

	*app = app_ad9083;

	return SUCCESS;
}

/**
 * @brief Free the resources allocated by app_ad9083_init().
 * @param app - App descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t app_ad9083_remove(struct app_ad9083 *app)
{
	if (!app)
		return FAILURE;

	free(app);

	return SUCCESS;
}
