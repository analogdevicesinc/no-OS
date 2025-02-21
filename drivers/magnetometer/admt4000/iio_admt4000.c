/***************************************************************************//**
 *   @file   iio_admt4000.c
 *   @brief  Implementation of IIO ADMT4000 driver.
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
#include "iio_admt4000.h"
#include "admt4000.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "iio.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
static int admt4000_iio_read_samples(void *dev, int16_t *buff,
                                     uint32_t samples);

static int admt4000_iio_read_scale(void *dev, char *buf, uint32_t len,
                                   const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_read_offset(void *dev, char *buf, uint32_t len,
                                    const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_update_channels(void* dev, uint32_t mask);

static int admt4000_iio_reg_read(struct admt4000_iio_dev *dev, uint32_t reg,
                                 uint32_t *readval);

static int admt4000_iio_reg_write(struct admt4000_iio_dev *dev, uint32_t reg,
                                  uint32_t *writeval);

static int admt4000_iio_store_page(void *dev, char *buf, uint32_t len,
                                   const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_show_page(void *dev, char *buf, uint32_t len,
                                  const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_store_seq_mode(void *dev, char *buf, uint32_t len,
                                       const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_show_seq_mode(void *dev, char *buf, uint32_t len,
                                      const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_store_conv_mode(void *dev, char *buf, uint32_t len,
                                        const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_show_conv_mode(void *dev, char *buf, uint32_t len,
                                       const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_store_angle_filt_en(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_show_angle_filt_en(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_store_h8_ctrl_en(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_show_h8_ctrl_en(void *dev, char *buf, uint32_t len,
                                        const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_store_sdp_gpio_ctrl(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_show_sdp_gpio_ctrl(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_store_sdp_gpio0_busy(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_show_sdp_gpio0_busy(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_store_sdp_pulse_coil_rs(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_show_sdp_pulse_coil_rs(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_store_throw_early_samples(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_show_throw_early_samples(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv);

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/

static struct iio_attribute admt4000_iio_attrs[] = {
    {
        .name = "page",
        .store = admt4000_iio_store_page,
        .show = admt4000_iio_show_page,
    },
    {
        .name = "sequencer_mode",
        .store = admt4000_iio_store_seq_mode,
        .show = admt4000_iio_show_seq_mode,
    },
    {
        .name = "angle_filt",
        .store = admt4000_iio_store_angle_filt_en,
        .show = admt4000_iio_show_angle_filt_en,
    },
    {
        .name = "conversion_mode",
        .store = admt4000_iio_store_conv_mode,
        .show = admt4000_iio_show_conv_mode,
    },
    {
        .name = "h8_ctrl",
        .store = admt4000_iio_store_h8_ctrl_en,
        .show = admt4000_iio_show_h8_ctrl_en,
    },
    {
        .name = "sdp_gpio_ctrl",
        .store = admt4000_iio_store_sdp_gpio_ctrl,
        .show = admt4000_iio_show_sdp_gpio_ctrl,
    },
    {
        .name = "sdp_gpio0_busy",
        .store = admt4000_iio_store_sdp_gpio0_busy,
        .show = admt4000_iio_show_sdp_gpio0_busy,
    },
    {
        .name = "sdp_coil_rs",
        .store = admt4000_iio_store_sdp_pulse_coil_rs,
        .show = admt4000_iio_show_sdp_pulse_coil_rs,
    },
    {
        .name = "throw_early_samples",
        .store = admt4000_iio_store_throw_early_samples,
        .show = admt4000_iio_show_throw_early_samples,
    },

    END_ATTRIBUTES_ARRAY
};

static struct iio_attribute admt4000_scan_attrs[] = {
    {
        .name = "scale",
        .show = admt4000_iio_read_scale,
    },
    {
        .name = "offset",
        .show = admt4000_iio_read_offset,
    },
    END_ATTRIBUTES_ARRAY
};

static struct scan_type admt4000_iio_absangle_scan_type = {
    .sign = 'u',
    .realbits = 10,
    .storagebits = 16,
    .shift = 0,
    .is_big_endian = false
};

static struct scan_type admt4000_iio_angle_scan_type = {
    .sign = 'u',
    .realbits = 12,
    .storagebits = 16,
    .shift = 0,
    .is_big_endian = false
};

static struct scan_type admt4000_iio_turns_scan_type = {
    .sign = 's',
    .realbits = 6,
    .storagebits = 16,
    .shift = 0,
    .is_big_endian = false
};

static struct iio_channel admt4000_channels[] = {
    {
        .ch_type = IIO_ROT, // absangle
        .channel = 0,
        .address = 0,
        .scan_index = 0,
        .attributes = admt4000_scan_attrs,
        .scan_type = &admt4000_iio_absangle_scan_type,
        .ch_out = false
    },
    {
        .ch_type = IIO_ANGL, // angle
        .channel = 1,
        .address = 1,
        .scan_index = 1,
        .attributes = admt4000_scan_attrs,
        .scan_type = &admt4000_iio_angle_scan_type,
        .ch_out = false
    },
    {
        .ch_type = IIO_COUNT,
        .channel = 2,
        .address = 2,
        .scan_index = 2,
        .attributes = admt4000_scan_attrs,
        .scan_type = &admt4000_iio_turns_scan_type,
        .ch_out = false
    },
    {
        .ch_type = IIO_TEMP,
        .channel = 3,
        .address = 3,
        .scan_index = 3,
        .attributes = admt4000_scan_attrs,
        .scan_type = &admt4000_iio_angle_scan_type,
        .ch_out = false
    },
};

static struct iio_device admt4000_iio_dev = {
    .num_ch = NO_OS_ARRAY_SIZE(admt4000_channels),
    .attributes = admt4000_iio_attrs,
    .channels = admt4000_channels,
    .debug_reg_read = (int32_t (*)())admt4000_iio_reg_read,
    .debug_reg_write = (int32_t (*)())admt4000_iio_reg_write,
    .pre_enable = (int32_t (*)())admt4000_iio_update_channels,
    .read_dev = (int32_t (*)()) admt4000_iio_read_samples,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/******************************************************************************/

/**
 * @brief Initializes the ADMT4000 IIO driver
 * @param iio_dev - The iio device structure.
 * @param init_param - Parameters for the initialization of iio_dev
 *
 * @return 0 in case of success, errno errors otherwise
 */
int admt4000_iio_init(struct admt4000_iio_dev **iio_dev,
                      struct admt4000_iio_dev_init_param *init_param)
{
    int ret;
    struct admt4000_iio_dev *descriptor;

    if (!init_param)
        return -EINVAL;

    descriptor = no_os_calloc(1, sizeof(*descriptor));
    if (!descriptor)
        return -ENOMEM;

    ret = admt4000_init(&descriptor->admt4000_desc, *(init_param->admt4000_dev_init));
    if (ret)
        goto init_err;

    descriptor->iio_dev = &admt4000_iio_dev;

    *iio_dev = descriptor;

    return 0;
init_err:
    no_os_free(descriptor);

    return ret;
}

/**
 * @brief Free resources allocated by the init function
 * @param desc - The iio device structure.
 *
 * @return 0 in case of success, errno errors otherwise
 */
int admt4000_iio_remove(struct admt4000_iio_dev *desc)
{
    int ret;

    ret = admt4000_remove(desc->admt4000_desc);
    if (ret)
        return ret;

    no_os_free(desc);

    return 0;
}

/**
 * @brief ADMT4000 reg read wrapper
 * @param dev - The iio device structure.
 * @param reg - Register address
 * @param readval - Register value
 *
 * @return 0 in case of success, errno errors otherwise
 */
static int admt4000_iio_reg_read(struct admt4000_iio_dev *dev, uint32_t reg,
                                 uint32_t *readval)
{
    if (reg > __UINT8_MAX__)
        return -EINVAL;

    return admt4000_read(dev->admt4000_desc, (uint8_t)reg,
                         (uint16_t *) readval, NULL);
}

/**
 * @brief ADMT4000 reg write wrapper
 * @param dev - The iio device structure.
 * @param reg - Register address
 * @param readval - Register value
 *
 * @return 0 in case of success, errno errors otherwise
 */
static int admt4000_iio_reg_write(struct admt4000_iio_dev *dev, uint32_t reg,
                                  uint32_t *writeval)
{
    if (!dev)
        return -ENODEV;

    if (writeval > __UINT16_MAX__)
        return -EINVAL;

    return admt4000_write(dev->admt4000_desc, (uint8_t)reg,
                          (uint16_t ) writeval);
}

/**
 * @brief Handles the read request for page attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int admt4000_iio_show_page(void *dev, char *buf, uint32_t len,
                                  const struct iio_ch_info *channel, intptr_t priv)
{
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;
    int ret;
    bool b_page;
    int32_t page = 0;

    iio_admt4000 = (struct admt4000_iio_dev *)dev;
    admt4000 = iio_admt4000->admt4000_desc;

    ret = admt4000_get_page(admt4000, &b_page);
    if (ret)
        return ret;

    if (!b_page)
        page = 2;
    else
        page = 0;

    return iio_format_value(buf, len, IIO_VAL_INT, 1, &page);
}

/**
 * @brief Handles the write request for page attribute.
 *
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int admt4000_iio_store_page(void *dev, char *buf, uint32_t len,
                                   const struct iio_ch_info *channel, intptr_t priv)
{
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;
    uint64_t page = no_os_str_to_uint32(buf);
    int ret;
    uint16_t temp = 0;

    if (page != 0 && page != 2)
        return -EINVAL;

    iio_admt4000 = (struct admt4000_iio_dev *)dev;
    admt4000 = iio_admt4000->admt4000_desc;

    if (page == 0)
        ret = admt4000_set_page(admt4000, false);
    else if (page == 2)
        ret = admt4000_set_page(admt4000, true);
    if (ret)
        return ret;

    return len;
}

/**
 * @brief Handles the read request for sequencer mode attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int admt4000_iio_show_seq_mode(void *dev, char *buf, uint32_t len,
                                      const struct iio_ch_info *channel, intptr_t priv)
{
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;
    int ret;
    int32_t seq = 0;

    iio_admt4000 = (struct admt4000_iio_dev *)dev;
    admt4000 = iio_admt4000->admt4000_desc;

    ret = admt4000_get_seq_mode(admt4000, &seq);
    if (ret)
        return ret;

    return iio_format_value(buf, len, IIO_VAL_INT, 1, &seq);
}

/**
 * @brief Handles the write request for sequencer mode attribute.
 *
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int admt4000_iio_store_seq_mode(void *dev, char *buf, uint32_t len,
                                       const struct iio_ch_info *channel, intptr_t priv)
{
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;
    uint64_t seq = no_os_str_to_uint32(buf);
    int ret;
    uint16_t temp = 0;

    if (seq != ADMT4000_MODE2 && seq != ADMT4000_MODE1)
        return -EINVAL;

    iio_admt4000 = (struct admt4000_iio_dev *)dev;
    admt4000 = iio_admt4000->admt4000_desc;

    ret = admt4000_set_seq_mode(admt4000, seq);
    if (ret)
        return ret;

    return len;
}

/**
 * @brief Handles the write request for conversion mode attribute.
 *
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int admt4000_iio_store_conv_mode(void *dev, char *buf, uint32_t len,
                                        const struct iio_ch_info *channel, intptr_t priv)
{
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;
    uint64_t conv_mode = no_os_str_to_uint32(buf);
    int ret;

    if (conv_mode != 0 && conv_mode != 1)
        return -EINVAL;

    iio_admt4000 = (struct admt4000_iio_dev *)dev;
    admt4000 = iio_admt4000->admt4000_desc;

    ret = admt4000_set_cnv_mode(admt4000, (bool) conv_mode);
    if (ret)
        return ret;

    return len;
}

/**
 * @brief Handles the read request for conversion mode attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int admt4000_iio_show_conv_mode(void *dev, char *buf, uint32_t len,
                                       const struct iio_ch_info *channel, intptr_t priv)
{
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;
    bool is_one_shot;
    uint16_t temp;
    int32_t conv_mode = 0;
    int ret;

    iio_admt4000 = (struct admt4000_iio_dev *)dev;
    admt4000 = iio_admt4000->admt4000_desc;

    ret = admt4000_get_cnv_mode(admt4000, &is_one_shot);
    if (ret)
        return ret;

    if (!is_one_shot)
        conv_mode = 0;
    else
        conv_mode = 1;

    return iio_format_value(buf, len, IIO_VAL_INT, 1, &conv_mode);
}

/**
 * @brief Handles the write request for angle filt enable attribute.
 *
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int admt4000_iio_store_angle_filt_en(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv)
{
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;
    uint64_t en = no_os_str_to_uint32(buf);
    int ret;

    if (en != 0 && en != 1)
        return -EINVAL;

    iio_admt4000 = (struct admt4000_iio_dev *)dev;
    admt4000 = iio_admt4000->admt4000_desc;

    ret = admt4000_set_angle_filt(admt4000, (bool)en);
    if (ret)
        return ret;

    return len;
}

/**
 * @brief Handles the read request for angle filt enable attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int admt4000_iio_show_angle_filt_en(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv)
{
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;
    int ret;
    bool temp;
    int32_t en = 0;

    iio_admt4000 = (struct admt4000_iio_dev *)dev;
    admt4000 = iio_admt4000->admt4000_desc;

    ret = admt4000_get_angle_filt(admt4000, &temp);
    if (ret)
        return ret;

    if (!temp)
        en = 0;
    else
        en = 1;

    return iio_format_value(buf, len, IIO_VAL_INT, 1, &en);
}

/**
 * @brief Handles the write request for h8 control enable attribute.
 *
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int admt4000_iio_store_h8_ctrl_en(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv)
{
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;
    uint64_t en = no_os_str_to_uint32(buf);
    int ret;

    if (en != 0 && en != 1)
        return -EINVAL;

    iio_admt4000 = (struct admt4000_iio_dev *)dev;
    admt4000 = iio_admt4000->admt4000_desc;

    ret = admt4000_set_h8_ctrl(admt4000, (bool)en);
    if (ret)
        return ret;

    return len;
}

/**
 * @brief Handles the read request for h8 control enable attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int admt4000_iio_show_h8_ctrl_en(void *dev, char *buf, uint32_t len,
                                        const struct iio_ch_info *channel, intptr_t priv)
{
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;
    int ret;
    bool temp;
    int32_t en = 0;

    iio_admt4000 = (struct admt4000_iio_dev *)dev;
    admt4000 = iio_admt4000->admt4000_desc;

    ret = admt4000_get_h8_ctrl(admt4000, &temp);
    if (ret)
        return ret;

    if (!temp)
        en = 0;
    else
        en = 1;

    return iio_format_value(buf, len, IIO_VAL_INT, 1, &en);
}

/**
 * @brief Handles the write request for gpio0_busy.
 *
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int admt4000_iio_store_sdp_gpio0_busy(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv)
{
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;

    iio_admt4000 = (struct admt4000_iio_dev *)dev;
    admt4000 = iio_admt4000->admt4000_desc;

    return 0;
}

/**
 * @brief Handles the read request for for gpio0_busy.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int admt4000_iio_show_sdp_gpio0_busy(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv)
{
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;
    int ret;
    uint8_t vals;

    iio_admt4000 = (struct admt4000_iio_dev *)dev;
    admt4000 = iio_admt4000->admt4000_desc;

    ret = admt4000_sdp_getval_gpio0_busy(admt4000, &vals);
    if (ret)
        return ret;

    return iio_format_value(buf, len, IIO_VAL_INT, 1, vals);
}

/**
 * @brief Handles the write request for h8 control enable attribute.
 *
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int admt4000_iio_store_sdp_gpio_ctrl(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv)
{
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;

    iio_admt4000 = (struct admt4000_iio_dev *)dev;
    admt4000 = iio_admt4000->admt4000_desc;

    return 0;
}

/**
 * @brief Handles the read request for h8 control enable attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int admt4000_iio_show_sdp_gpio_ctrl(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv)
{
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;

    iio_admt4000 = (struct admt4000_iio_dev *)dev;
    admt4000 = iio_admt4000->admt4000_desc;

    return 0;
}

/**
 * @brief Handles the write request for pulsing coil_rs.
 *
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int admt4000_iio_store_sdp_pulse_coil_rs(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv)
{
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;
    int ret;

    iio_admt4000 = (struct admt4000_iio_dev *)dev;
    admt4000 = iio_admt4000->admt4000_desc;

    ret = admt4000_sdp_pulse_coil_rs(admt4000);

    return ret;
}

/**
 * @brief Handles the read request for pulsing coil_rs.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int admt4000_iio_show_sdp_pulse_coil_rs(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv)
{
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;

    iio_admt4000 = (struct admt4000_iio_dev *)dev;
    admt4000 = iio_admt4000->admt4000_desc;

    return 0;
}

/***************************************************************************//**
 * @brief Handles the read request for scale attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the reading procedure.
 * 					In case of success, the size of the read data is returned.
*******************************************************************************/
static int admt4000_iio_read_scale(void *dev, char *buf, uint32_t len,
                                   const struct iio_ch_info *channel, intptr_t priv)
{
    int32_t vals[2];
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;

    if (!dev)
        return -EINVAL;

    iio_admt4000 = dev;

    if (!iio_admt4000->admt4000_desc)
        return -EINVAL;

    admt4000 = iio_admt4000->admt4000_desc;

    switch (channel->type) {
    case IIO_ROT: // absangle
        vals[0] = 360;
        vals[1] = 10;

        return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, vals);
    case IIO_ANGL: // angle
        vals[0] = 360;
        vals[1] = 12;

        return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, vals);
    case IIO_COUNT:
        vals[0] = 1;

        return iio_format_value(buf, len, IIO_VAL_INT, 1, vals);
    case IIO_TEMP:
        vals[0] = 100;
        vals[1] = 1632;

        return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
    default:
        return -EINVAL;
    }
}

/***************************************************************************//**
 * @brief Handles the read request for offset attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the reading procedure.
 * 					In case of success, the size of the read data is returned.
*******************************************************************************/
static int admt4000_iio_read_offset(void *dev, char *buf, uint32_t len,
                                    const struct iio_ch_info *channel, intptr_t priv)
{
    int32_t vals[2];
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;

    if (!dev)
        return -EINVAL;

    iio_admt4000 = dev;

    if (!iio_admt4000->admt4000_desc)
        return -EINVAL;

    admt4000 = iio_admt4000->admt4000_desc;

    switch (channel->type) {
    case IIO_ROT: // absangle
    case IIO_ANGL: // angle
    case IIO_COUNT:
        vals[0] = 0;

        return iio_format_value(buf, len, IIO_VAL_INT, 1, vals);
    case IIO_TEMP:
        vals[0] = 1150;

        return iio_format_value(buf, len, IIO_VAL_INT, 1, vals);
    default:
        return -EINVAL;
    }
}

/**
 * @brief Reads the number of given samples for the selected channels
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param samples - Number of samples to be returned
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int admt4000_iio_read_samples(void *dev, int16_t *buff,
                                     uint32_t samples)
{
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;
    bool is_one_shot, cnv;
    uint16_t angle[2];
    int i, turns, ret;

    if (!dev)
        return -EINVAL;

    iio_admt4000 = (struct admt4000_iio_dev *)dev;

    if (!iio_admt4000->admt4000_desc)
        return -EINVAL;

    admt4000 = iio_admt4000->admt4000_desc;

    /* Set CNV bits to high as initialization */
    ret = admt4000_set_cnv(admt4000, true);
    if (ret)
        return ret;

    /* Conversion mode can be set in attribute page */
    ret = admt4000_get_cnv_mode(admt4000, &is_one_shot);
    if (ret)
        return ret;

    /* For continuous mode, set cnv bits low */
    if (!is_one_shot) {
        ret = admt4000_set_cnv(admt4000, false);
        if (ret)
            return ret;

        if (admt4000->is_throw_early_samples) {
            /* Read dummy data points */
            /* Based on observed behavior in iio scope */
            for (i = 0; i < 21; i++) { // working dump: 21 samples
                ret = admt4000_get_raw_turns_and_angle(admt4000, &turns, angle);
                if (ret)
                    return ret;
            }
        }
    }

    for (i = 0; i < samples * iio_admt4000->no_of_active_channels;) {
        if (is_one_shot) {
            while (1) { // make sure that cnv bits are false
                ret = admt4000_set_cnv(admt4000, false);
                if (ret)
                    break;

                ret = admt4000_get_cnv(admt4000, &cnv);
                if (ret)
                    break;

                if (!cnv)
                    break;
            }

            no_os_udelay(800); // previous working delay 250uS
        }

        ret = admt4000_get_raw_turns_and_angle(admt4000, &turns, angle);
        if (ret)
            break;

        if (iio_admt4000->active_channels & NO_OS_BIT(0)) {
            buff[i] = (int16_t) angle[0];
            i++;
        }
        if (iio_admt4000->active_channels & NO_OS_BIT(1)) {
            buff[i] = (int16_t) angle[1];
            i++;
        }
        if (iio_admt4000->active_channels & NO_OS_BIT(2)) {
            if (turns > ADMT4000_TURN_CNT_THRES)
                turns = (int16_t)(turns) - ADMT4000_TURN_CNT_TWOS;

            buff[i] = (int16_t) turns;
            i++;
        }
        if (iio_admt4000->active_channels & NO_OS_BIT(3)) {
            ret = admt4000_get_temp(admt4000, &buff[i], true);
            i++;
        }

        if (is_one_shot) {
            while (1) { // make sure cnv bits are high
                ret = admt4000_set_cnv(admt4000, true);
                if (ret)
                    break;

                ret = admt4000_get_cnv(admt4000, &cnv);
                if (ret)
                    break;

                if (cnv)
                    break;
            }
        }

        ret = admt4000_clear_all_faults(admt4000);
        if (ret)
            break;
    }

    ret = admt4000_set_cnv(admt4000, true);
    if (ret)
        return ret;

    return samples;
}

/*******************************************************************************
 * @brief Updates the number of active channels and the total number of
 * 		  active channels
 *
 * @param dev  - The iio device structure.
 * @param mask - Mask of the active channels
 *
 * @return ret - Result of the updating procedure.
*******************************************************************************/
static int admt4000_iio_update_channels(void *dev, uint32_t mask)
{
    struct admt4000_iio_dev *iio_admt4000;

    if (!dev)
        return -EINVAL;

    iio_admt4000 = (struct admt4000_iio_dev *) dev;

    iio_admt4000->active_channels = mask;

    iio_admt4000->no_of_active_channels = no_os_hweight32(mask);

    return 0;
}

/**
 * @brief Handles the write request for sequencer mode attribute.
 *
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int admt4000_iio_store_throw_early_samples(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv)
{
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;
    int ret;

    uint64_t throw_early_samples = no_os_str_to_uint32(buf);
    uint16_t temp = 0;

    if (throw_early_samples != 0 && throw_early_samples != 1)
        return -EINVAL;

    iio_admt4000 = (struct admt4000_iio_dev *)dev;
    admt4000 = iio_admt4000->admt4000_desc;

    switch (throw_early_samples)
    {
    case 0:
        admt4000->is_throw_early_samples = false;
        break;
    case 1:
        admt4000->is_throw_early_samples = true;
        break;
    }

    return len;
}

/**
 * @brief Handles the read request for h8 control enable attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int admt4000_iio_show_throw_early_samples(void *dev, char *buf, uint32_t len,
        const struct iio_ch_info *channel, intptr_t priv)
{
    struct admt4000_iio_dev *iio_admt4000;
    struct admt4000_dev *admt4000;
    int32_t throw_early_samples;

    iio_admt4000 = (struct admt4000_iio_dev *)dev;
    admt4000 = iio_admt4000->admt4000_desc;

    switch(admt4000->is_throw_early_samples)
    {
    case false:
        throw_early_samples = 0;
        break;
    case true:
        throw_early_samples = 1;
        break;
    }

    return iio_format_value(buf, len, IIO_VAL_INT, 1, &throw_early_samples);
}
