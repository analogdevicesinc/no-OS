/***************************************************************************//**
 *   @file   iio_tmc5240_control.c
 *   @brief  Implementation of IIO TMC5240 driver.
 *   @author Jose Ramon San Buenaventura (jose.sanbuenaventura@analog.com)
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

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "no_os_spi.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "iio.h"
#include "iio_tmc5240.h"
#include "tmc5240_wrapper.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
static int tmc5240_iio_reg_read(struct tmc5240_iio_dev *dev, uint32_t reg,
				uint32_t *readval);

static int tmc5240_iio_reg_write(struct tmc5240_iio_dev *dev, uint32_t reg,
				 uint32_t writeval);

static int tmc5240_iio_store_amax(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv);

static int tmc5240_iio_show_amax(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);

static int tmc5240_iio_store_vmax(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv);

static int tmc5240_iio_show_vmax(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);

static int tmc5240_iio_store_dmax(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv);

static int tmc5240_iio_show_dmax(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);

static int tmc5240_iio_store_stop(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv);

static int tmc5240_iio_store_target_pos(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv);

static int tmc5240_iio_show_target_pos(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv);

static int tmc5240_iio_show_current_pos(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv);

static int tmc5240_iio_store_ramp_mode(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv);

static int tmc5240_iio_show_ramp_mode(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv);

static int tmc5240_iio_show_ramp_mode_avail(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
static const char* const tmc5240_ramp_mode_avail[] = {
	[TMC5240_RAMP_MODE_POSITION] = "position",
	[TMC5240_RAMP_MODE_VPOSITIVE] = "velocity_positive",
	[TMC5240_RAMP_MODE_VNEGATIVE] = "velocity_negative",
	[TMC5240_RAMP_MODE_HOLD] = "hold",
	NULL
};

static struct iio_attribute tmc5240_iio_attrs[] = {
	{
		.name = "amax",
		.store = tmc5240_iio_store_amax,
		.show = tmc5240_iio_show_amax,
	},
	{
		.name = "vmax",
		.store = tmc5240_iio_store_vmax,
		.show = tmc5240_iio_show_vmax,
	},
	{
		.name = "dmax",
		.store = tmc5240_iio_store_dmax,
		.show = tmc5240_iio_show_dmax,
	},
	{
		.name = "stop",
		.store = tmc5240_iio_store_stop,
		.show = NULL,
	},
	{
		.name = "target_pos",
		.store = tmc5240_iio_store_target_pos,
		.show = tmc5240_iio_show_target_pos,
	},
	{
		.name = "current_pos",
		.store = NULL,
		.show = tmc5240_iio_show_current_pos,
	},
	{
		.name = "ramp_mode",
		.store = tmc5240_iio_store_ramp_mode,
		.show = tmc5240_iio_show_ramp_mode,
	},
	{
		.name = "ramp_mode_available",
		.show = tmc5240_iio_show_ramp_mode_avail,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device tmc5240_iio_dev = {
	.attributes = tmc5240_iio_attrs,
	.debug_reg_read = (int32_t (*)())tmc5240_iio_reg_read,
	.debug_reg_write = (int32_t (*)())tmc5240_iio_reg_write,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/******************************************************************************/

/**
 * @brief Initializes the tmc5240 IIO driver
 * @param iio_dev - The iio device structure.
 * @param init_param - Parameters for the initialization of iio_dev
 *
 * @return 0 in case of success, errno errors otherwise
 */
int tmc5240_iio_init(struct tmc5240_iio_dev **iio_dev,
		     struct tmc5240_iio_dev_init_param *init_param)
{
	int i, ret;
	struct tmc5240_iio_dev *descriptor;

	if (!init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = tmc5240_init(&descriptor->tmc5240_dev, init_param->tmc5240_init_param);
	if (ret)
		goto init_spi_err;

	/* Initialize using TMC-API init */
	// tmc5240_init(&descriptor->tmc5240_dev, 1, my_config, def_reg_rst);

	descriptor->position = ret;

	descriptor->iio_dev = &tmc5240_iio_dev;

	*iio_dev = descriptor;

	return 0;

init_spi_err:
	tmc5240_remove(descriptor->tmc5240_dev);
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function
 * @param desc - The iio device structure.
 *
 * @return 0 in case of success, errno errors otherwise
 */
int tmc5240_iio_remove(struct tmc5240_iio_dev *desc)
{
	int ret;

	ret = tmc5240_remove(desc->tmc5240_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}

/**
 * @brief tmc5240 reg read wrapper
 * @param dev - The iio device structure.
 * @param reg - Register address
 * @param readval - Register value
 *
 * @return 0 in case of success, errno errors otherwise
 */
static int tmc5240_iio_reg_read(struct tmc5240_iio_dev *dev, uint32_t reg,
				uint32_t *readval)
{
	if (!dev)
		return -ENODEV;

	return tmc5240_read(dev->tmc5240_dev, reg, readval);
}

/**
 * @brief tmc5240 reg write wrapper
 * @param dev - The iio device structure.
 * @param reg - Register address
 * @param readval - Register value
 *
 * @return 0 in case of success, errno errors otherwise
 */
static int tmc5240_iio_reg_write(struct tmc5240_iio_dev *dev, uint32_t reg,
				 uint32_t writeval)
{
	if (!dev)
		return -ENODEV;

	return tmc5240_write(dev->tmc5240_dev, reg, writeval);

}

/**
 * @brief Handles the read request for AMAX attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int tmc5240_iio_show_amax(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	struct tmc5240_iio_dev *iio_tmc5240;
	int ret;
	int32_t val;


	if (!dev)
		return -ENODEV;

	iio_tmc5240 = (struct tmc5240_iio_dev *)dev;

	ret = tmc5240_get_amax(iio_tmc5240->tmc5240_dev, &val);

	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Handles the write request for AMAX attribute.
 *
 * @param device - The iio device structure
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int tmc5240_iio_store_amax(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	struct tmc5240_iio_dev *iio_tmc5240;
	int ret;
	int32_t amax = no_os_str_to_int32(buf);

	if (!dev)
		return -ENODEV;

	iio_tmc5240 = (struct tmc5240_iio_dev *)dev;

	ret = tmc5240_set_amax(iio_tmc5240->tmc5240_dev, amax);

	if (ret)
		return ret;


	iio_tmc5240->amax = amax;

	return len;
}

/**
 * @brief Handles the read request for VMAX attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int tmc5240_iio_show_vmax(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	struct tmc5240_iio_dev *iio_tmc5240;
	int ret;
	int32_t val;


	if (!dev)
		return -ENODEV;

	iio_tmc5240 = (struct tmc5240_iio_dev *)dev;

	ret = tmc5240_get_vmax(iio_tmc5240->tmc5240_dev, &val);

	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Handles the write request for VMAX attribute.
 *
 * @param device - The iio device structure
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int tmc5240_iio_store_vmax(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	struct tmc5240_iio_dev *iio_tmc5240;
	int ret;
	int32_t vmax = no_os_str_to_int32(buf);

	if (!dev)
		return -ENODEV;

	iio_tmc5240 = (struct tmc5240_iio_dev *)dev;

	ret = tmc5240_set_vmax(iio_tmc5240->tmc5240_dev, vmax);

	if (ret)
		return ret;

	iio_tmc5240->vmax = vmax;

	return len;
}

/**
 * @brief Handles the read request for DMAX attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int tmc5240_iio_show_dmax(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	struct tmc5240_iio_dev *iio_tmc5240;
	int ret;
	int32_t val;


	if (!dev)
		return -ENODEV;

	iio_tmc5240 = (struct tmc5240_iio_dev *)dev;

	ret = tmc5240_get_dmax(iio_tmc5240->tmc5240_dev, &val);

	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Handles the write request for DMAX attribute.
 *
 * @param device - The iio device structure
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int tmc5240_iio_store_dmax(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	struct tmc5240_iio_dev *iio_tmc5240;
	int ret;
	int32_t dmax = no_os_str_to_int32(buf);

	if (!dev)
		return -ENODEV;

	iio_tmc5240 = (struct tmc5240_iio_dev *)dev;

	ret = tmc5240_set_dmax(iio_tmc5240->tmc5240_dev, dmax);

	if (ret)
		return ret;

	return len;
}

/**
 * @brief Handles the write request for motor disable attribute.
 *
 * @param device - The iio device structure
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int tmc5240_iio_store_stop(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	struct tmc5240_iio_dev *iio_tmc5240;
	int32_t dis = no_os_str_to_int32(buf);
	int ret;

	if (!dev)
		return -ENODEV;

	/* Buffer must contain "1" to issue disable */
	if (dis != 1)
		return -EINVAL;

	iio_tmc5240 = (struct tmc5240_iio_dev *)dev;

	ret = tmc5240_stop(iio_tmc5240->tmc5240_dev);
	if (ret)
		return ret;

	return len;
}

/**
 * @brief Handles the write request for target_pos attribute.
 *
 * @param device - The iio device structure
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int tmc5240_iio_store_target_pos(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv)
{
	struct tmc5240_iio_dev *iio_tmc5240;
	int32_t pos = no_os_str_to_int32(buf);
	int ret;

	if (!dev)
		return -ENODEV;

	iio_tmc5240 = (struct tmc5240_iio_dev *)dev;

	ret = tmc5240_set_target_pos(iio_tmc5240->tmc5240_dev, pos, iio_tmc5240->vmax);
	if (ret)
		return ret;

	return len;
}

/**
 * @brief Handles the write request for target_pos attribute.
 *
 * @param device - The iio device structure
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int tmc5240_iio_show_target_pos(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv)
{
	struct tmc5240_iio_dev *iio_tmc5240;
	int ret;
	int32_t val;

	if (!dev)
		return -ENODEV;

	iio_tmc5240 = (struct tmc5240_iio_dev *)dev;

	ret = tmc5240_get_target_pos(iio_tmc5240->tmc5240_dev, &val);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Handles the write request for current_pos attribute.
 *
 * @param device - The iio device structure
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int tmc5240_iio_show_current_pos(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv)
{
	struct tmc5240_iio_dev *iio_tmc5240;
	int ret;
	int32_t val;

	if (!dev)
		return -ENODEV;

	iio_tmc5240 = (struct tmc5240_iio_dev *)dev;

	ret = tmc5240_get_current_pos(iio_tmc5240->tmc5240_dev, &val);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Handles the write request for ramp_mode attribute.
 *
 * @param device - The iio device structure
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int tmc5240_iio_store_ramp_mode(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv)
{
	struct tmc5240_iio_dev *iio_tmc5240;
	int ret;
	int i;

	if (!dev)
		return -ENODEV;

	iio_tmc5240 = (struct tmc5240_iio_dev *)dev;

	for (i = 0; i < NO_OS_ARRAY_SIZE(tmc5240_ramp_mode_avail); i++) {
		if (!strcmp(buf, tmc5240_ramp_mode_avail[i])) {
			return tmc5240_set_ramp_mode(iio_tmc5240->tmc5240_dev, i);
		}
	}

	return -EINVAL;
}

/**
 * @brief Handles the write request for ramp_mode attribute.
 *
 * @param device - The iio device structure
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int tmc5240_iio_show_ramp_mode(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv)
{
	struct tmc5240_iio_dev *iio_tmc5240;
	int ret;
	int32_t val;

	if (!dev)
		return -ENODEV;

	iio_tmc5240 = (struct tmc5240_iio_dev *)dev;

	ret = tmc5240_get_ramp_mode(iio_tmc5240->tmc5240_dev, &val);
	if (ret)
		return ret;

	return sprintf(buf, "%s", tmc5240_ramp_mode_avail[val]);
	//return iio_format_value(buf, len, IIO_VAL_CHAR, 1, tmc5240_ramp_mode_avail[val]);
}

/**
 * @brief Handles the write request for ramp modes attribute.
 *
 * @param device - The iio device structure
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int tmc5240_iio_show_ramp_mode_avail(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int length = 0;
	int i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(tmc5240_ramp_mode_avail); i++)
		length += sprintf(buf + length, "%s ", tmc5240_ramp_mode_avail[i]);

	return length;
}

