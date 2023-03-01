/***************************************************************************//**
 *   @file   ad9083.c
 *   @brief  Implementation of ad9083 Driver.
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

#include <errno.h>
#include <stdlib.h>
#include "ad9083.h"
#include "adi_ad9083_hal.h"
#include "no_os_error.h"
#include <inttypes.h>
#include "no_os_delay.h"
#include "no_os_clk.h"
#include "uc_settings.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
extern int32_t adi_ad9083_jtx_startup(adi_ad9083_device_t *device,
				      adi_cms_jesd_param_t *jtx_param);

extern int32_t adi_ad9083_jesd_tx_link_digital_reset(adi_ad9083_device_t
		*device,
		uint8_t reset);

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define CHIPID_AD9083		0x00EA
#define CHIPID_MASK		0xFFFF
#define SPI_IN_OUT_BUFF_SZ	0x3
#define MAX_REG_ADDR		0x1000
#define SPI_READ		0x80

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Read register.
 * @param device - ad9083 device.
 * @param reg - Address of the register.
 * @param readval - Value of the register.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ad9083_reg_get(struct ad9083_phy *device, uint32_t reg,
		       uint8_t *readval)
{
	int32_t ret;
	uint8_t data[SPI_IN_OUT_BUFF_SZ] = {0};

	if (reg < MAX_REG_ADDR) {
		data[0] = (reg >> 8) | SPI_READ;
		data[1] = reg;
		ret = no_os_spi_write_and_read(device->spi_desc, data, SPI_IN_OUT_BUFF_SZ);
		if (ret != 0)
			return ret;

		*readval = data[2];
	}
	return 0;
}

/**
 * @brief Write register.
 * @param device - ad9083 device.
 * @param reg - Address of the register.
 * @param writeval - Value of the register.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ad9083_reg_set(struct ad9083_phy *device, uint32_t reg,
		       uint8_t writeval)
{
	int32_t ret;
	uint8_t data[SPI_IN_OUT_BUFF_SZ] = {0};

	if (reg < MAX_REG_ADDR) {
		data[0] = reg >> 8;
		data[1] = reg;
		data[2] = writeval;
		ret = no_os_spi_write_and_read(device->spi_desc, data, SPI_IN_OUT_BUFF_SZ);
		if (ret != 0)
			return ret;
	}
	return 0;
}

/**
 * Spi write and read compatible with ad9083 API
 * @param user_data
 * @param in_data - Pointer to array with the data to be sent on the SPI
 * @param out_data - Pointer to array where the data to which the SPI will be written
 * @param size_bytes - The size in bytes allocated for each of the indata and outdata arrays.
 * @return 0 for success, any non-zero value indicates an error
 */
static int32_t ad9083_spi_xfer(void *user_data, uint8_t *in_data,
			       uint8_t *out_data, uint32_t size_bytes)
{
	struct ad9083_phy *phy = user_data;
	uint8_t data[6];
	uint8_t bytes_number;
	int32_t ret;
	int32_t i;

	if (size_bytes > 6)
		return -1;

	bytes_number = size_bytes;

	for (i = 0; i < bytes_number; i++)
		data[i] = in_data[i];

	ret = no_os_spi_write_and_read(phy->spi_desc, data, bytes_number);
	if (ret != 0)
		return -1;

	for (i = 0; i < bytes_number; i++)
		out_data[i] = data[i];

	return 0;
}

/**
 * Log write
 * @param user_data
 * @param log_type - Log source type.
 * @param message - Message.
 * @param argp - Argp.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ad9083_log_write(void *user_data, int32_t log_type, const char *message,
			 va_list argp)
{
	char logMessage[160];

	vsnprintf(logMessage, sizeof(logMessage), message, argp);

	switch (log_type) {
	case ADI_CMS_LOG_NONE:
		break;
	case ADI_CMS_LOG_MSG:
		break;
	case ADI_CMS_LOG_WARN:
		printf("%s\n", logMessage);
		break;
	case ADI_CMS_LOG_ERR:
		printf("%s\n", logMessage);
		break;
	case ADI_CMS_LOG_SPI:
		break;
	case ADI_CMS_LOG_API:
		break;
	case ADI_CMS_LOG_ALL:
		printf(logMessage);
		break;
	}

	return 0;
}

/**
 * Delay microseconds, compatible with ad9083 API
 * Performs a blocking or sleep delay for the specified time in microseconds
 * @param user_data
 * @param us - time to delay/sleep in microseconds.
 * @return 0 in case of success, -1 otherwise.
 */
static int ad9083_udelay(void *user_data, unsigned int us)
{
	no_os_udelay(us);

	return 0;
}

/**
 * Reset pin control
 * @param user_data
 * @param enable - State.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ad9083_reset_pin_ctrl(void *user_data, uint8_t enable)
{
	struct ad9083_phy *phy = user_data;

	return no_os_gpio_set_value(phy->gpio_reset, enable);
}

/**
 * ad9083 link reset
 * @param device - The device structure.
 * @param uc - Configuration selection.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t ad9083_link_reset(struct ad9083_phy *device, uint8_t uc)
{
	struct uc_settings *uc_settings = get_uc_settings();
	adi_cms_jesd_param_t *jtx_param = &uc_settings->jtx_param[uc];
	int32_t ret;

	if (jtx_param->jesd_subclass == 1) {
		adi_ad9083_hal_bf_set(&device->adi_ad9083,
				      BF_JTX_TPL_SYSREF_CLR_PHASE_ERR_INFO, 1);
		adi_ad9083_hal_bf_set(&device->adi_ad9083,
				      BF_JTX_TPL_SYSREF_CLR_PHASE_ERR_INFO, 0);
	}

	ret = adi_ad9083_jesd_tx_link_digital_reset(&device->adi_ad9083, 1);
	if (ret != 0)
		return ret;

	no_os_mdelay(1);
	ret = adi_ad9083_jesd_tx_link_digital_reset(&device->adi_ad9083, 0);
	if (ret != 0)
		return ret;

	no_os_mdelay(1);

	return 0;
}

/**
 * Setup ad9083 device
 * @param device - The device structure.
 * @param uc - Configuration selection.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t ad9083_setup(struct ad9083_phy *device, uint8_t uc)
{
	struct uc_settings *uc_settings = get_uc_settings();
	uint64_t *clk_hz = uc_settings->clk_hz[uc];
	uint32_t vmax = uc_settings->vmax[uc];
	uint32_t fc = uc_settings->fc[uc];
	uint8_t rterm = uc_settings->rterm[uc];
	uint32_t en_hp = uc_settings->en_hp[uc];
	uint32_t backoff = uc_settings->backoff[uc];
	uint32_t finmax = uc_settings->finmax[uc];
	uint64_t *nco_freq_hz = uc_settings->nco_freq_hz[uc];
	uint8_t *decimation = uc_settings->decimation[uc];
	uint8_t nco0_datapath_mode = uc_settings->nco0_datapath_mode[uc];
	adi_cms_jesd_param_t *jtx_param = &uc_settings->jtx_param[uc];
	int32_t ret;

	/* software reset, resistor is not mounted */
	ret = adi_ad9083_device_reset(&device->adi_ad9083, AD9083_SOFT_RESET);
	if (ret != 0)
		return ret;

	ret = adi_ad9083_device_init(&device->adi_ad9083);
	if (ret != 0)
		return ret;

	ret = adi_ad9083_device_clock_config_set(&device->adi_ad9083, clk_hz[2],
			clk_hz[0]);
	if (ret != 0)
		return ret;

	ret = adi_ad9083_rx_adc_config_set(&device->adi_ad9083, vmax, fc,
					   rterm, en_hp, backoff, finmax);
	if (ret != 0)
		return ret;

	ret = adi_ad9083_rx_datapath_config_set(&device->adi_ad9083,
						nco0_datapath_mode, decimation, nco_freq_hz);
	if (ret != 0)
		return ret;

	if (jtx_param->jesd_subclass == 1) {
		ret = adi_ad9083_hal_bf_set(&device->adi_ad9083, BF_SYSREF_RESYNC_MODE_INFO, 1);
		if (ret)
			return ret;
		ret = adi_ad9083_hal_bf_set(&device->adi_ad9083, BF_JTX_SYSREF_FOR_STARTUP_INFO,
					    1);
		if (ret)
			return ret;
		ret = adi_ad9083_hal_bf_set(&device->adi_ad9083, BF_JTX_SYSREF_FOR_RELINK_INFO,
					    1);
		if (ret)
			return ret;
		ret = adi_ad9083_hal_bf_set(&device->adi_ad9083, BF_SYSREF_RESYNC_MODE_INFO, 1);
		if (ret)
			return ret;
		ret = adi_ad9083_hal_bf_set(&device->adi_ad9083, 0x00000D40, 0x00000101, 0);
		if (ret)
			return ret;
	}

	ret = adi_ad9083_jtx_startup(&device->adi_ad9083, jtx_param);
	if (ret != 0)
		return ret;

	return 0;
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad9083_init(struct ad9083_phy **device,
		    struct ad9083_init_param *init_param)
{
	adi_cms_chip_id_t chip_id;
	struct ad9083_phy *phy;
	uint8_t api_rev[3];
	int32_t ret;

	phy = (struct ad9083_phy *)no_os_calloc(1, sizeof(*phy));
	if (!phy)
		return -ENOMEM;

	ret = no_os_gpio_get_optional(&phy->gpio_reset, init_param->gpio_reset);
	if (ret != 0)
		goto error_1;
	ret = no_os_gpio_get_optional(&phy->gpio_pd, init_param->gpio_pd);
	if (ret != 0)
		goto error_2;

	no_os_gpio_direction_output(phy->gpio_reset, NO_OS_GPIO_HIGH);

	ret = no_os_spi_init(&phy->spi_desc, init_param->spi_init);
	if (ret != 0)
		goto error_3;

	phy->adi_ad9083.hal_info.user_data = phy;
	phy->adi_ad9083.hal_info.delay_us = ad9083_udelay;
	phy->adi_ad9083.hal_info.reset_pin_ctrl = ad9083_reset_pin_ctrl;
	phy->adi_ad9083.hal_info.sdo = SPI_SDIO;
	phy->adi_ad9083.hal_info.msb = SPI_MSB_FIRST;
	phy->adi_ad9083.hal_info.addr_inc = SPI_ADDR_INC_AUTO;
	phy->adi_ad9083.hal_info.spi_xfer = ad9083_spi_xfer;
	phy->adi_ad9083.hal_info.log_write = ad9083_log_write;

	ret = adi_ad9083_device_chip_id_get(&phy->adi_ad9083, &chip_id);
	if (ret != 0) {
		printf("%s: chip_id failed (%"PRId32")\n", __func__, ret);
		goto error_4;
	}

	if ((chip_id.prod_id & CHIPID_MASK) != CHIPID_AD9083) {
		printf("%s: Unrecognized CHIP_ID 0x%X\n", __func__,
		       chip_id.prod_id);
		ret = -1;
		goto error_4;
	}

	ret = ad9083_setup(phy, init_param->uc);
	if (ret != 0) {
		printf("%s: ad9083_setup failed (%"PRId32")\n", __func__, ret);
		goto error_4;
	}

	ret = ad9083_link_reset(phy, init_param->uc);
	if (ret != 0) {
		printf("%s: ad9083_link failed (%"PRId32")\n", __func__, ret);
		goto error_4;
	}

	if (init_param->jesd_rx_clk) {
		ret = no_os_clk_enable(init_param->jesd_rx_clk);
		if (ret != 0) {
			printf("Failed to enable JESD204 link: %d\n", ret);
			goto error_4;
		}
	}

	ret = adi_ad9083_device_api_revision_get(&phy->adi_ad9083, &api_rev[0],
			&api_rev[1], &api_rev[2]);
	if (ret != 0)
		goto error_4;

	printf("AD9083 Rev. %u Grade %u (API %u.%u.%u) probed\n",
	       chip_id.dev_revision, chip_id.prod_grade,
	       api_rev[0], api_rev[1], api_rev[2]);
	*device = phy;

	return 0;

error_4:
	no_os_spi_remove(phy->spi_desc);
error_3:
	no_os_gpio_remove(phy->gpio_pd);
error_2:
	no_os_gpio_remove(phy->gpio_reset);
error_1:
	if (phy)
		no_os_free(phy);

	return ret;
}

/**
 * @brief Free the resources allocated by ad9083_init().
 * @param dev - Device descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ad9083_remove(struct ad9083_phy *dev)
{
	int32_t ret;

	if (!dev)
		return -1;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret != 0)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_pd);
	if (ret != 0)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_reset);
	if (ret != 0)
		return ret;

	no_os_free(dev);

	return 0;
}
