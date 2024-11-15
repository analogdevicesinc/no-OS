/***************************************************************************//**
*   @file   ade9153A.c
*   @brief  Implementation of ADE9153A Driver.
*   @author Radu Etz (radu.etz@analog.com)
********************************************************************************
* Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "ade9153a.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "no_os_delay.h"
#include "no_os_units.h"
#include "no_os_alloc.h"
#include "no_os_crc16.h"
#include "no_os_print_log.h"
#include <stdlib.h>
#include <errno.h>
#include <math.h>

NO_OS_DECLARE_CRC16_TABLE(ade9153a_crc16);

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Select comms interface SPI.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_set_interface_spi(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENOMEM;

	/* reset the ADE and initialize the SPI:
		SS - Low and SCK - High at reset */
	if (dev->gpio_ss) {
		ret = no_os_gpio_direction_output(dev->gpio_ss,
						  NO_OS_GPIO_LOW);
		if (ret)
			return ret;
	}

	if (dev->gpio_sck) {
		ret = no_os_gpio_direction_output(dev->gpio_sck,
						  NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
	}

	return ade9153a_hw_reset(dev);

}

/**
 * @brief Select comms interface Serial.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_set_interface_serial(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENOMEM;

	/* reset the ADE and initialize the Serial:
		SS - High and SCK - Low at reset */
	if (dev->gpio_ss) {
		ret = no_os_gpio_direction_output(dev->gpio_ss,
						  NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
	}

	if (dev->gpio_sck) {
		ret = no_os_gpio_direction_output(dev->gpio_sck,
						  NO_OS_GPIO_LOW);
		if (ret)
			return ret;
	}

	return ade9153a_hw_reset(dev);

}

/**
 * @brief GPIO interrupt handler for data ready.
 * @param dev - The device structure.
 */
static void ade9153a_irq_handler(void *dev)
{
	int ret;
	/* ADE9153A device descriptor */
	struct ade9153a_dev *desc = dev;
	/* data read */
	uint8_t reg_val;

	/* Disable interrupt while reading data. */
	ret = no_os_irq_disable(desc->irq_ctrl,
				desc->gpio_rdy->number);
	if (ret)
		return;

	/* Reenable interrupt */
	ret = no_os_irq_enable(desc->irq_ctrl,
			       desc->gpio_rdy->number);

	return;
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */

int ade9153a_init(struct ade9153a_dev **device,
		  struct ade9153a_init_param init_param)
{
	int ret;
	/* ADE9153A device structure */
	struct ade9153a_dev *dev;
	/* value read from register */
	uint32_t reg_val;

	if (!init_param.irq_ctrl)
		return -EINVAL;

	dev = (struct ade9153a_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	struct no_os_callback_desc irq_cb = {
		.callback = ade9153a_irq_handler,
		.ctx = dev,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ
	};

	if (init_param.drdy_callback)
		irq_cb.callback = init_param.drdy_callback;

	ret = no_os_gpio_get_optional(&dev->gpio_rdy, init_param.gpio_rdy);
	if (ret)
		goto error_gpio_rdy;

	if (dev->gpio_rdy) {
		ret = no_os_gpio_direction_input(dev->gpio_rdy);
		if (ret)
			goto error_gpio_rdy;
	}

	ret = no_os_irq_register_callback(init_param.irq_ctrl,
					  dev->gpio_rdy->number, &irq_cb);
	if (ret)
		goto error_gpio_rdy;

	dev->irq_cb = irq_cb;

	ret = no_os_irq_trigger_level_set(init_param.irq_ctrl,
					  dev->gpio_rdy->number, NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		goto error_irq;

	ret = no_os_irq_set_priority(init_param.irq_ctrl, dev->gpio_rdy->number, 3);
	if (ret)
		goto error_irq;

	ret = no_os_irq_disable(init_param.irq_ctrl,
				dev->gpio_rdy->number);
	if (ret)
		goto error_irq;

	dev->irq_ctrl = init_param.irq_ctrl;

	/* Create the CRC-16 lookup table for polynomial ADE9153A_CRC8_POLY */
	no_os_crc16_populate_msb(ade9153a_crc16, ADE9153A_CRC16_POLY);

	ret = no_os_gpio_get_optional(&dev->gpio_reset,
				      init_param.gpio_reset);
	if (ret)
		goto error_irq;

	if (dev->gpio_reset) {
		ret = no_os_gpio_direction_output(dev->gpio_reset, NO_OS_GPIO_HIGH);
		if (ret)
			goto error_gpio_reset;
	}

	/* SPI Initialization */
	if (init_param.spi_en) {
		ret = no_os_gpio_get_optional(&dev->gpio_ss,
					      init_param.gpio_ss);
		if (ret)
			goto error_gpio_reset;

		ret = no_os_gpio_get_optional(&dev->gpio_sck,
					      init_param.gpio_sck);
		if (ret)
			goto error_spi;

		ret = ade9153a_set_interface_spi(dev);
		if (ret)
			goto error_spi;

		ret = no_os_spi_init(&dev->spi_desc,
				     init_param.spi_init);
		if (ret)
			goto error_spi;
	}

	no_os_mdelay(ADE9153A_RESET_DEL);

	ret = ade9153a_run(dev);
	if (ret)
		goto error_spi;

	/* Read version product */
	ret = ade9153a_version_product(dev, &reg_val);
	if (ret)
		goto error_spi;

	if (reg_val != ADE9153A_VERSION) {
		pr_info("ERROR !!!\n");
		goto error_spi;
	}

	pr_info("Product Version %x \n", reg_val);

	*device = dev;

	return 0;

error_spi:
	no_os_spi_remove(dev->spi_desc);
error_gpio_reset:
	no_os_gpio_remove(dev->gpio_reset);
error_irq:
	no_os_irq_unregister_callback(init_param.irq_ctrl, dev->gpio_rdy->number,
				      &irq_cb);
error_gpio_rdy:
	no_os_gpio_remove(dev->gpio_rdy);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief ADE9153A setup.
 * @param dev - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 */
int ade9153a_setup(void *dev, struct ade9153a_init_param init_param)
{
	int ret;

	if (!dev)
		return -ENODEV;

	// ai swap init
	ret = ade9153a_ai_swap(dev, init_param.ai_swap);
	if (ret)
		return ret;
	// set PGA gain
	ret = ade9153a_ai_gain(dev, init_param.ai_pga_gain);
	if (ret)
		return ret;
	// set hpf freq
	ret = ade9153a_hpf_crn(dev, init_param.hpf_crn);
	if (ret)
		return ret;
	// set energy accumulation mode
	ret = ade9153a_selfreq(dev, init_param.freq);
	if (ret)
		return ret;
	// set vlevel
	ret = ade9153a_write(dev, ADE9153A_REG_VLEVEL, init_param.vlevel);
	if (ret)
		return ret;
	// enable egyrdy_int
	ret = ade9153a_enable_egyrdy_int(dev);
	if (ret)
		return ret;
	// initialize for proper operation
	ret = ade9153a_compmode(dev);
	if (ret)
		return ret;
	// set vdiv rsmall
	ret = ade9153a_write(dev, ADE9153A_REG_VDIV_RSMALL, init_param.rsmall);
	if (ret)
		return ret;
	// enable energy and power accumulator
	ret = ade9153a_egy_pwr_en(dev);
	if (ret)
		return ret;
	//enable energy reset feature
	ret = ade9153a_rd_rst_en(dev);
	if (ret)
		return ret;
	// set energy accumulate no samples
	ret = ade9153a_write(dev, ADE9153A_REG_EGY_TIME, init_param.no_samples);
	if (ret)
		return ret;
	// AIGAIN -1 (IAP-IAN swap) AIGAIN = -1*2^28
	ret = ade9153a_write(dev, ADE9153A_REG_AIGAIN, init_param.ai_gain);
	if (ret)
		return ret;
	// burst read config based on burst_en in dev structure
	ret = ade9153a_burst_en(dev);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Read device register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data read from 32bit register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_read(struct ade9153a_dev *dev, uint16_t reg_addr,
		  uint32_t *reg_data)
{
	int ret;
	/* CRC computed for read response messages */
	uint16_t crc16;
	/* CRC received with read response messages */
	uint16_t recv_crc;
	/* no of read bytes: 6 - for 16 bit reg 8 - for 32 bit reg */
	uint8_t no_of_read_bytes = 8;
	/* no of bytes from received buffer not used for CRC*/
	uint8_t bytes_not_used = 4;
	/* offset of data read in the read buffer */
	uint8_t data_byte_offset = 2;
	/* position of byte in buffer to start read to */
	uint8_t position = 2;
	/* buffer for data read */
	uint8_t buff[8] = { 0 };
	/* index */
	uint8_t i;
	/* register addres */
	uint32_t addr;

	if (!dev)
		return -ENODEV;
	if (!reg_data)
		return -EINVAL;

	/* reset command buffer */
	for (i = 0; i < 8; i++)
		buff[i] = 0;

	addr = (uint16_t) no_os_field_prep(NO_OS_GENMASK(16,4), reg_addr);
	no_os_put_unaligned_be16(addr, &buff);
	buff[1] = buff[1] | ADE9153A_SPI_READ;

	if ((reg_addr > ADE9153A_START_16ADDR ) & (reg_addr < ADE9153A_END_16ADDR)) {
		no_of_read_bytes = ADE9153A_NO_BYTES_R_16;
		ret = no_os_spi_write_and_read(dev->spi_desc, buff,
					       no_of_read_bytes);
		if (ret)
			return ret;
		*reg_data = (uint16_t)no_os_get_unaligned_be16(&buff[position]);
	} else {
		no_of_read_bytes = ADE9153A_NO_BYTES_R_32;
		ret = no_os_spi_write_and_read(dev->spi_desc, buff,
					       no_of_read_bytes);
		if (ret)
			return ret;
		*reg_data = (uint32_t)no_os_get_unaligned_be32(&buff[position]);
	}

	/* check received CRC, if burst disabled */
	if (!(dev->burst_en)) {
		crc16 = no_os_crc16(ade9153a_crc16, &buff[data_byte_offset],
				    no_of_read_bytes - bytes_not_used,
				    ADE9153A_CRC16_INIT_VAL);

		recv_crc = no_os_get_unaligned_be16(&buff[data_byte_offset + no_of_read_bytes -
								     bytes_not_used]);

		if (recv_crc != crc16) {
			/* if we read 0s on SPI then there is no communication */
			if (!recv_crc)
				return -ENODEV;
			/* the application should handle this result */
			return -EPROTO;
		}
	}

	return 0;
}

/**
 * @brief Write device register.
 * @param dev- The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_write(struct ade9153a_dev *dev, uint16_t reg_addr,
		   uint32_t reg_data)
{
	int ret;
	/* buffer for data write */
	uint8_t buff[8] = {0};
	/* register addres */
	uint16_t addr;
	/* data to be written */
	uint32_t data;
	/* offset of data to be written in the buffer */
	uint8_t data_byte_offset = 2;
	int i;

	if (!dev)
		return -ENODEV;

	addr = (uint16_t) no_os_field_prep(NO_OS_GENMASK(16,4), reg_addr);
	no_os_put_unaligned_be16(addr, &buff);

	if ((reg_addr > ADE9153A_START_16ADDR ) & (reg_addr < ADE9153A_END_16ADDR)) {
		no_os_put_unaligned_be16(reg_data, &buff[data_byte_offset]);
		ret = no_os_spi_write_and_read(dev->spi_desc, buff, ADE9153A_NO_BYTES_W_16);
		if (ret)
			return ret;
	} else {
		no_os_put_unaligned_be32(reg_data, &buff[data_byte_offset]);
		ret = no_os_spi_write_and_read(dev->spi_desc, buff, ADE9153A_NO_BYTES_W_32);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Update specific register bits.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Specific bits mask.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ade9153a_update_bits(struct ade9153a_dev *dev, uint16_t reg_addr,
				uint32_t mask, uint32_t reg_data)
{
	int ret;
	/* data to be written */
	uint32_t data;

	if (!dev)
		return -ENODEV;

	ret = ade9153a_read(dev, reg_addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_data & mask;

	return ade9153a_write(dev, reg_addr, data);
}

/**
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_remove(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_rdy);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_reset);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_ss);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_sck);
	if (ret)
		return ret;

	ret = no_os_irq_unregister_callback(dev->irq_ctrl, dev->gpio_rdy->number,
					    &dev->irq_cb);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}

/**
 * @brief Reset the device using SW reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_sw_reset(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG1, ADE9153A_SWRST_MSK,
				   no_os_field_prep(ADE9153A_SWRST_MSK, ENABLE));
	if (ret)
		return ret;

	/* Wait for device to initialize */
	no_os_mdelay(ADE9153A_RESET_DEL);

	return 0;
}

/**
 * @brief Reset the device using HW reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_hw_reset(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	if (dev->gpio_reset) {
		ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_LOW);
		if (ret)
			return ret;
	}
	no_os_mdelay(ADE9153A_RESET_DEL);

	if (dev->gpio_reset)
		ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	/* Wait for device to initialize */
	no_os_mdelay(ADE9153A_RESET_DEL);

	return 0;
}

/**
 * @brief Lock device.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_wr_lock(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = ade9153a_write(dev, ADE9153A_REG_WR_LOCK, ADE9153A_LOCK_KEY);
	return ret;
}

/**
 * @brief Unlock device.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_wr_unlock(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = ade9153a_write(dev, ADE9153A_REG_WR_LOCK, ADE9153A_UNLOCK_KEY);
	return ret;
}

/**
 * @brief Version product
 * @param dev - The device structure.
 * @param data_read - The version product read value
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_version_product(struct ade9153a_dev *dev, uint32_t *data_read)
{
	int ret;
	/* version product */
	uint32_t version;

	if (!dev)
		return -ENODEV;
	if (!data_read)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_VERSION_PRODUCT, &version);
	if (ret)
		return ret;

	*data_read = version;

	return 0;
}

/**
 * @brief Disable the low pass filter in the fundamental reactive power datapath.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_lpf_rp_disable(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG0, ADE9153A_DISRPLPF_MSK,
				   no_os_field_prep(ADE9153A_DISRPLPF_MSK, ENABLE));
	return ret;
}

/**
 * @brief Disable the low-pass filter in the total active power datapath.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_lpf_ap_disable(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG0, ADE9153A_DISAPLPF_MSK,
				   no_os_field_prep(ADE9153A_DISAPLPF_MSK, ENABLE));
	return ret;
}

/**
 * @brief Use the nominal phase voltage rms, VNOM, in the computation
 * 			of the Phase A total apparent power.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_vnoma_enable(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG0, ADE9153A_VNOMA_EN_MSK,
				   no_os_field_prep(ADE9153A_VNOMA_EN_MSK, ENABLE));
	return ret;
}

/**
 * @brief Use x_WAV waveforms after the high-pass filter and phase
 * 			compensation for the RMS_OC calculation.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_rms_oc_src_x_wav_enable(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG0, ADE9153A_RMS_OC_SRC_MSK,
				   no_os_field_prep(ADE9153A_RMS_OC_SRC_MSK, DISABLE));
	return ret;
}

/**
 * @brief Use ADC samples, before the high-pass filter for
 * 			the RMS_OC calculation.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_rms_oc_src_adc_samples_enable(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG0, ADE9153A_RMS_OC_SRC_MSK,
				   no_os_field_prep(ADE9153A_RMS_OC_SRC_MSK, ENABLE));
	return ret;
}

/**
 * @brief ZX data source after hpf and phase compensation
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_zx_src_after_hpf_enable(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG0, ADE9153A_ZX_SRC_SEL_MSK,
				   no_os_field_prep(ADE9153A_ZX_SRC_SEL_MSK, DISABLE));
	return ret;
}

/**
 * @brief ZX data source before hpf and phase compensation
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_zx_src_before_hpf_enable(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG0, ADE9153A_ZX_SRC_SEL_MSK,
				   no_os_field_prep(ADE9153A_ZX_SRC_SEL_MSK, ENABLE));
	return ret;
}

/**
 * @brief Current channel B integrator enable
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_i_ch_b_int_enable(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG0, ADE9153A_INTEN_BI_MSK,
				   no_os_field_prep(ADE9153A_INTEN_BI_MSK, ENABLE));
	return ret;
}

/**
 * @brief Disable hpf for all channels
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_hpf_disable(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG0, ADE9153A_HPFDIS_MSK,
				   no_os_field_prep(ADE9153A_HPFDIS_MSK, ENABLE));
	return ret;
}

/**
 * @brief Set PGA gain for current channel B
 * @param dev - The device structure.
 * @param gain - value of the PGA gain
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_bi_pgagain_set(struct ade9153a_dev *dev,
			    enum ade9153a_bi_gain_e gain)
{
	int ret;

	if (!dev)
		return -ENODEV;

	switch (gain) {
	case ADE9153A_PGA_CHB_GAIN_2:
		ret = ade9153a_write(dev, ADE9153A_REG_BI_PGAGAIN, ADE9153A_PGA_CHB_GAIN_2);
		break;
	case ADE9153A_PGA_CHB_GAIN_4:
		ret = ade9153a_write(dev, ADE9153A_REG_BI_PGAGAIN, ADE9153A_PGA_CHB_GAIN_4);
		break;
	default:
		ret = ade9153a_write(dev, ADE9153A_REG_BI_PGAGAIN, ADE9153A_PGA_CHB_GAIN_1);
		break;
	}
	return ret;
}

/**
 * @brief Auto calibration config
 * @param dev - The device structure.
 * @param ch - channel enabled for autocalibration
 * @param mode - current channel mode
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_auto_calibration_cfg(struct ade9153a_dev *dev,
				  enum ade9153a_acal_ch_e ch, enum ade9153a_acalmode_e mode)
{
	int ret;

	if (!dev)
		return -ENODEV;

	switch (ch) {
	case AUTOCAL_BI:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_MS_ACAL_CFG,
					   ADE9153A_AUTOCAL_BI_MSK,
					   no_os_field_prep(ADE9153A_AUTOCAL_BI_MSK, ENABLE));
		if (ret)
			return ret;
		ret = ade9153a_update_bits(dev, ADE9153A_REG_MS_ACAL_CFG,
					   ADE9153A_ACALMODE_BI_MSK,
					   no_os_field_prep(ADE9153A_ACALMODE_BI_MSK, mode));
		if (ret)
			return ret;
		break;
	case AUTOCAL_AI:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_MS_ACAL_CFG,
					   ADE9153A_AUTOCAL_AI_MSK,
					   no_os_field_prep(ADE9153A_AUTOCAL_AI_MSK, ENABLE));
		if (ret)
			return ret;
		ret = ade9153a_update_bits(dev, ADE9153A_REG_MS_ACAL_CFG,
					   ADE9153A_ACALMODE_AI_MSK,
					   no_os_field_prep(ADE9153A_ACALMODE_AI_MSK, mode));
		if (ret)
			return ret;
		break;
	default:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_MS_ACAL_CFG,
					   ADE9153A_AUTOCAL_AV_MSK,
					   no_os_field_prep(ADE9153A_AUTOCAL_AV_MSK, ENABLE));
		if (ret)
			return ret;
		break;
	}
	return 0;
}

/**
 * @brief Auto calibration run
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_auto_calibration_run(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = ade9153a_update_bits(dev, ADE9153A_REG_MS_ACAL_CFG,
				   ADE9153A_ACAL_MODE_MSK,
				   no_os_field_prep(ADE9153A_ACAL_MODE_MSK, ENABLE));
	if (ret)
		return ret;
	ret = ade9153a_update_bits(dev, ADE9153A_REG_MS_ACAL_CFG, ADE9153A_ACAL_RUN_MSK,
				   no_os_field_prep(ADE9153A_ACAL_RUN_MSK, ENABLE));
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Auto calibration stop
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_auto_calibration_stop(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = ade9153a_update_bits(dev, ADE9153A_REG_MS_ACAL_CFG,
				   ADE9153A_ACAL_MODE_MSK,
				   no_os_field_prep(ADE9153A_ACAL_MODE_MSK, DISABLE));
	return ret;
}

/**
 * @brief mSure status
 * @param dev - The device structure.
 * @param status - stores the mSure status 0 - not ready run 1 - ready to run
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_msure_status(struct ade9153a_dev *dev, uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_MS_STATUS_CURRENT, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(ADE9153A_MS_SYSRDYP_MSK),
				 &reg_val);

	return 0;
}

/**
 * @brief Ipeak channel phase
 * @param dev - The device structure.
 * @param ch - stores the ch that generates the IPEAKVAL 1 - for ch A, 2 - for ch B, 0 - none
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_ip_phase(struct ade9153a_dev *dev, uint8_t *ch)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!ch)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_IPEAK, &reg_val);
	if (ret)
		return ret;

	if (no_os_test_bit(no_os_find_first_set_bit(ADE9153A_IPPHASE_MSK & 0x01),
			   &reg_val))
		*ch = 0x01;
	else if (no_os_test_bit(no_os_find_first_set_bit(ADE9153A_IPPHASE_MSK & 0x02),
				&reg_val))
		*ch = 0x02;
	else
		*ch = 0x00;

	return 0;
}

/**
 * @brief Ipeak val
 * @param dev - The device structure.
 * @param val - Ipeak val (xI_WAV/2^5)
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_ipk_val(struct ade9153a_dev *dev, uint32_t *val)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!val)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_IPEAK, &reg_val);
	if (ret)
		return ret;
	*val = no_os_get_unaligned_be24(&reg_val);

	return 0;
}

/**
 * @brief Vpeak val
 * @param dev - The device structure.
 * @param val - Vpeak val (AV_WAV/2^5)
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_vpk_val(struct ade9153a_dev *dev, uint32_t *val)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!val)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_VPEAK, &reg_val);
	if (ret)
		return ret;
	*val = no_os_get_unaligned_be24(&reg_val);

	return 0;
}

/**
 * @brief Get interrupt indicator from STATUS register.
 * @param dev - The device structure.
 * @param msk - Interrupt mask.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_int_status(struct ade9153a_dev *dev, uint32_t msk,
			    uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_STATUS, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(msk), &reg_val);

	return 0;
}

/**
 * @brief Get PF ready indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_pf_rdy(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_PF_RDY_MSK, status);
}

/**
 * @brief Get CRC change indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_crc_chg(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_CRC_CHG_MSK, status);
}

/**
 * @brief Get CRC done indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_crc_done(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_CRC_DONE_MSK, status);
}

/**
 * @brief Get zero crossing timout on V ch indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_zxtoav(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_ZXTOAV_MSK, status);
}

/**
 * @brief Get zero crossing detect on I ch B indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_zxbi(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_ZXBI_MSK, status);
}

/**
 * @brief Get zero crossing detect on I ch A indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_zxai(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_ZXAI_MSK, status);
}

/**
 * @brief Get zero crossing detect on V ch indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_zxav(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_ZXAV_MSK, status);
}

/**
 * @brief Get reset done indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_rstdone(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_RSTDONE_MSK, status);
}

/**
 * @brief Get fundamental reactive energy no load condition indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_fvarnl(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_FVARNL_MSK, status);
}

/**
 * @brief Get total apparent energy no load condition indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_vanl(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_VANL_MSK, status);
}

/**
 * @brief Get total active energy no load condition indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_wattnl(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_WATTNL_MSK, status);
}

/**
 * @brief Get new temperature reading ready indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_temp_rdy(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_TEMP_RDY_MSK, status);
}

/**
 * @brief Get RMS_OC values update indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_rms_oc_rdy(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_RMS_OC_RDY_MSK, status);
}

/**
 * @brief Get power values registers update indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_pwrrdy(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_PWRRDY_MSK, status);
}

/**
 * @brief Get new waveform samples ready indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_dready(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_DREADY_MSK, status);
}

/**
 * @brief Get power values egy ready indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_egyrdy(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_EGYRDY_MSK, status);
}

/**
 * @brief Get CF2 pulse issued indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_cf2(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_CF2_MSK, status);
}

/**
 * @brief Get CF1 pulse issued indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_cf1(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_CF1_MSK, status);
}

/**
 * @brief Get CF2 polarity change indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_cf2_chg(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_REVPCF2_MSK, status);
}

/**
 * @brief Get CF1 polarity change indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_cf1_chg(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_REVPCF1_MSK, status);
}

/**
 * @brief Get RPA (reactive power) sign change indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_rpa_chg_sgn(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_REVRPA_MSK, status);
}

/**
 * @brief Get APA (active power) sign change indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_apa_chg_sgn(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	return ade9153a_get_int_status(dev, ADE9153A_REVAPA_MSK, status);
}

/**
 * @brief Clear PF ready int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_pf_rdy(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_PF_RDY_MSK,
				    no_os_field_prep(ADE9153A_PF_RDY_MSK, ENABLE));

}

/**
 * @brief Clear CRC change int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_crc_chg(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_CRC_CHG_MSK,
				    no_os_field_prep(ADE9153A_CRC_CHG_MSK, ENABLE));

}

/**
 * @brief Clear CRC done int mask.
 * @param dev - The device structure..
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_crc_done(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_CRC_DONE_MSK,
				    no_os_field_prep(ADE9153A_CRC_DONE_MSK, ENABLE));

}

/**
 * @brief Clear zero crossing timout on V ch int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_zxtoav(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_ZXTOAV_MSK,
				    no_os_field_prep(ADE9153A_ZXTOAV_MSK, ENABLE));

}

/**
 * @brief Clear zero crossing detect on I ch B int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_zxbi(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_ZXBI_MSK,
				    no_os_field_prep(ADE9153A_ZXBI_MSK, ENABLE));

}

/**
 * @brief Clear zero crossing detect on I ch A int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_zxai(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_ZXAI_MSK,
				    no_os_field_prep(ADE9153A_ZXAI_MSK, ENABLE));

}

/**
 * @brief Clear zero crossing detect on V ch int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_zxav(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_ZXAV_MSK,
				    no_os_field_prep(ADE9153A_ZXAV_MSK, ENABLE));

}

/**
 * @brief Clear reset done int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_rstdone(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_RSTDONE_MSK,
				    no_os_field_prep(ADE9153A_RSTDONE_MSK, ENABLE));

}

/**
 * @brief Clear fundamental reactive energy no load condition int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_fvarnl(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_FVARNL_MSK,
				    no_os_field_prep(ADE9153A_FVARNL_MSK, ENABLE));

}

/**
 * @brief Clear total apparent energy no load condition int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_vanl(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_VANL_MSK,
				    no_os_field_prep(ADE9153A_VANL_MSK, ENABLE));

}

/**
 * @brief Clear total active energy no load condition int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_wattnl(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_WATTNL_MSK,
				    no_os_field_prep(ADE9153A_WATTNL_MSK, ENABLE));

}

/**
 * @brief Clear new temperature reading ready int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_temp_rdy(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_TEMP_RDY_MSK,
				    no_os_field_prep(ADE9153A_TEMP_RDY_MSK, ENABLE));

}

/**
 * @brief Clear RMS_OC values update int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_rms_oc_rdy(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_RMS_OC_RDY_MSK,
				    no_os_field_prep(ADE9153A_RMS_OC_RDY_MSK, ENABLE));

}

/**
 * @brief Clear power values registers update int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_pwrrdy(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_PWRRDY_MSK,
				    no_os_field_prep(ADE9153A_PWRRDY_MSK, ENABLE));

}

/**
 * @brief Clear new waveform samples ready int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_dready(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_DREADY_MSK,
				    no_os_field_prep(ADE9153A_DREADY_MSK, ENABLE));

}

/**
 * @brief Clear power values egy ready int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_egyrdy(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_EGYRDY_MSK,
				    no_os_field_prep(ADE9153A_EGYRDY_MSK, ENABLE));

}

/**
 * @brief Clear CF2 pulse issued int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_cf2(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_CF2_MSK,
				    no_os_field_prep(ADE9153A_CF2_MSK, ENABLE));

}

/**
 * @brief Clear CF1 pulse issued int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_cf1(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_CF1_MSK,
				    no_os_field_prep(ADE9153A_CF1_MSK, ENABLE));

}

/**
 * @brief Clear CF2 polarity change int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_cf2_chg(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_REVPCF2_MSK,
				    no_os_field_prep(ADE9153A_REVPCF2_MSK, ENABLE));
}

/**
 * @brief Clear CF1 polarity change int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_cf1_chg(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_REVPCF1_MSK,
				    no_os_field_prep(ADE9153A_REVPCF1_MSK, ENABLE));
}

/**
 * @brief Clear RPA (reactive power) sign change int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_rpa_chg_sgn(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_REVRPA_MSK,
				    no_os_field_prep(ADE9153A_REVRPA_MSK, ENABLE));
}

/**
 * @brief Clear APA (active power) sign change int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_apa_chg_sgn(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_STATUS, ADE9153A_REVAPA_MSK,
				    no_os_field_prep(ADE9153A_REVAPA_MSK, ENABLE));
}

/**
 * @brief Get chip status indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_chip_stat(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_get_int_status(dev, ADE9153A_CHIP_STAT_MSK, status);
}

/**
 * @brief Clear chip status int.
 * @param dev - The device structure.
 * @param reg_val - register value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_chip_stat(struct ade9153a_dev *dev, uint32_t *reg_val)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_read(dev, ADE9153A_CHIP_STAT_MSK, &reg_val);
}

/**
 * @brief Get event status indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_event_stat(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_get_int_status(dev, ADE9153A_EVENT_STAT_MSK, status);
}

/**
 * @brief Clear event status int.
 * @param dev - The device structure.
 * @param reg_val - register value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_event_stat(struct ade9153a_dev *dev, uint32_t *reg_val)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_read(dev, ADE9153A_EVENT_STAT_MSK, &reg_val);
}

/**
 * @brief Get MS status indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_ms_stat(struct ade9153a_dev *dev, uint8_t *status)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_get_int_status(dev, ADE9153A_MS_STAT_MSK, status);
}

/**
 * @brief Clear MS status int.
 * @param dev - The device structure.
 * @param reg_val - register value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_clear_ms_stat(struct ade9153a_dev *dev, uint32_t *reg_val)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_read(dev, ADE9153A_MS_STAT_MSK, &reg_val);
}

/**
 * @brief Enable/disable interrupt.
 * @param dev - The device structure.
 * @param reg_addr - MASK register address.
 * @param int_msk - Interrupt mask.
 * @param en - Enable/Disable.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_control_interrupt(struct ade9153a_dev *dev, uint16_t reg_addr,
			       uint32_t int_msk, uint8_t en)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, reg_addr,
				    int_msk, no_os_field_prep(int_msk, en));
}

/**
 * @brief Enable an interrupt when any bit in CHIP_STATUS reg is set
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_chip_stat_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_CHIP_STAT_MSK, ENABLE);
}

/**
 * @brief Disable CHIP_STAT interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_chip_stat_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_CHIP_STAT_MSK, DISABLE);
}

/**
 * @brief Enable an interrupt when any bit in EVENT_STATUS reg is set
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_event_stat_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_EVENT_STAT_MSK, ENABLE);
}

/**
 * @brief Disable EVENT_STAT interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_event_stat_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_EVENT_STAT_MSK, DISABLE);
}

/**
 * @brief Enable an interrupt when any bit in MS_STATUS_IRQ reg is set
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_ms_stat_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_MS_STAT_MSK, ENABLE);
}

/**
 * @brief Disable MS_STAT interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_ms_stat_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_MS_STAT_MSK, DISABLE);
}

/**
 * @brief Enable pf ready interrupt
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_pf_rdy_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_PF_RDY_MSK, ENABLE);
}

/**
 * @brief Disable pf ready interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_pf_rdy_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_PF_RDY_MSK, DISABLE);
}

/**
 * @brief Enable crc change interrupt
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_crc_chg_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_CRC_CHG_MSK, ENABLE);
}

/**
 * @brief Disable crc change interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_crc_chg_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_CRC_CHG_MSK, DISABLE);
}

/**
 * @brief Enable crc done interrupt
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_crc_done_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_CRC_DONE_MSK, ENABLE);
}

/**
 * @brief Disable crc done interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_crc_done_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_CRC_DONE_MSK, DISABLE);
}

/**
 * @brief Enable zero corssing timeout on voltage ch interrupt
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_zxtoav_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_ZXTOAV_MSK, ENABLE);
}

/**
 * @brief Disable zero corssing timeout on voltage ch interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_zxtoav_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_ZXTOAV_MSK, DISABLE);
}

/**
 * @brief Enable zero corssing detected on I ch B interrupt
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_zxbi_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_ZXBI_MSK, ENABLE);
}

/**
 * @brief Disable zero corssing detected on I ch B interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_zxbi_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_ZXBI_MSK, DISABLE);
}

/**
 * @brief Enable zero corssing detected on I ch A interrupt
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_zxai_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_ZXAI_MSK, ENABLE);
}

/**
 * @brief Disable zero corssing detected on I ch A interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_zxai_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_ZXAI_MSK, DISABLE);
}

/**
 * @brief Enable zero corssing detected on V ch interrupt
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_zxav_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_ZXAV_MSK, ENABLE);
}

/**
 * @brief Disable zero corssing detected on V ch interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_zxav_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_ZXAV_MSK, DISABLE);
}

/**
 * @brief Enable fundamental reactive energy no load condition interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_fvarnl_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_FVARNL_MSK, ENABLE);
}

/**
 * @brief Disable fundamental reactive energy no load condition interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_fvarnl_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_FVARNL_MSK, DISABLE);
}

/**
 * @brief Enable total apparent energy no load condition interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_vanl_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_VANL_MSK, ENABLE);
}

/**
 * @brief Disable total apparent energy no load condition interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_vanl_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_VANL_MSK, DISABLE);
}

/**
 * @brief Enable total active energy no load condition interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_wattnl_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_WATTNL_MSK, ENABLE);
}

/**
 * @brief Disable total active energy no load condition interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_wattnl_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_WATTNL_MSK, DISABLE);
}

/**
 * @brief Enable new temperature reading interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_temp_rdy_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_TEMP_RDY_MSK, ENABLE);
}

/**
 * @brief Disable new temperature reading interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_temp_rdy_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_TEMP_RDY_MSK, DISABLE);
}

/**
 * @brief Enable RMS_OC values update interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_rms_oc_rdy_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_RMS_OC_RDY_MSK, ENABLE);
}

/**
 * @brief Disable RMS_OC values update interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_rms_oc_rdy_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_RMS_OC_RDY_MSK, DISABLE);
}

/**
 * @brief Enable PWRRDY interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_pwrrdy_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_PWRRDY_MSK, ENABLE);
}

/**
 * @brief Disable PWRRDY interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_pwrrdy_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_PWRRDY_MSK, DISABLE);
}

/**
 * @brief Enable data ready interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_dready_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_DREADY_MSK, ENABLE);
}

/**
 * @brief Disable data ready interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_dready_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_DREADY_MSK, DISABLE);
}

/**
 * @brief Enable EGYRDY interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_egyrdy_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_EGYRDY_MSK, ENABLE);
}

/**
 * @brief Disable EGYRDY interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_egyrdy_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_EGYRDY_MSK, DISABLE);
}

/**
 * @brief Enable CF2 interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_cf2_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_CF2_MSK, ENABLE);
}

/**
 * @brief Disable CF2 interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_cf2_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_CF2_MSK, DISABLE);
}

/**
 * @brief Enable CF1 interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_cf1_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_CF1_MSK, ENABLE);
}

/**
 * @brief Disable CF1 interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_cf1_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_CF1_MSK, DISABLE);
}

/**
 * @brief Enable CF2 polarity change sign interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_revpcf2_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_REVPCF2_MSK, ENABLE);
}

/**
 * @brief Disable CF2 polarity change sign interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_revpcf2_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_REVPCF2_MSK, DISABLE);
}

/**
 * @brief Enable CF1 polarity change sign interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_revpcf1_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_REVPCF1_MSK, ENABLE);
}

/**
 * @brief Disable CF1 polarity change sign interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_revpcf1_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_REVPCF1_MSK, DISABLE);
}

/**
 * @brief Enable Phase A fundamental reactive power changed sign interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_revrpa_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_REVRPA_MSK, ENABLE);
}

/**
 * @brief Disable Phase A fundamental reactive power changed sign interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_revrpa_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_REVRPA_MSK, DISABLE);
}

/**
 * @brief Enable Phase A total active power changed sign interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_enable_revapa_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_REVAPA_MSK, ENABLE);
}

/**
 * @brief Disable Phase A total active power changed sign interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_disable_revapa_int(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_control_interrupt(dev, ADE9153A_REG_MASK,
					  ADE9153A_REVAPA_MSK, DISABLE);
}

/**
 * @brief Overcurrent detection threshold level val
 * @param dev - The device structure.
 * @param val - OI_LVL value
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_oi_lvl_val(struct ade9153a_dev *dev, uint32_t *val)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!val)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_OI_LVL, &reg_val);
	if (ret)
		return ret;
	*val = reg_val & (uint32_t)(ADE9153A_OILVL_VAL_MSK);

	return 0;
}

/**
 * @brief Ch A overcurrent val
 * @param dev - The device structure.
 * @param val - ch A overcurrent value
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_oia_val(struct ade9153a_dev *dev, uint32_t *val)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!val)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_OIA, &reg_val);
	if (ret)
		return ret;
	*val = reg_val & (uint32_t)(ADE9153A_OIA_VAL_MSK);

	return 0;
}

/**
 * @brief Ch B overcurrent val
 * @param dev - The device structure.
 * @param val - ch B overcurrent value
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_oib_val(struct ade9153a_dev *dev, uint32_t *val)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!val)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_OIB, &reg_val);
	if (ret)
		return ret;
	*val = reg_val & (uint32_t)(ADE9153A_OIB_VAL_MSK);

	return 0;
}

/**
 * @brief Vlevel val
 * @param dev - The device structure.
 * @param val - Vlevel value
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_vlevel_val(struct ade9153a_dev *dev, uint32_t *val)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!val)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_VLEVEL, &reg_val);
	if (ret)
		return ret;
	*val = reg_val & (uint32_t)(ADE9153A_VLEVEL_VAL_MSK);

	return 0;
}

/**
 * @brief Voltage value during dip condition
 * @param dev - The device structure.
 * @param val - dipa value
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_dipa_val(struct ade9153a_dev *dev, uint32_t *val)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!val)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_DIPA, &reg_val);
	if (ret)
		return ret;
	*val = reg_val & (uint32_t)(ADE9153A_DIPA_VAL_MSK);

	return 0;
}

/**
 * @brief Voltage value during swell condition
 * @param dev - The device structure.
 * @param val - swella value
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_swella_val(struct ade9153a_dev *dev, uint32_t *val)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!val)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_SWELLA, &reg_val);
	if (ret)
		return ret;
	*val = reg_val & (uint32_t)(ADE9153A_SWELLA_VAL_MSK);

	return 0;
}

/**
 * @brief Get phnoload status.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_get_phnoload_status(struct ade9153a_dev *dev, uint8_t *status)
{
	int ret;
	/* register value read */
	uint8_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_PHNOLOAD, &reg_val);
	if (ret)
		return ret;

	if (no_os_test_bit(no_os_find_first_set_bit(ADE9153A_AFVARNL_MSK), &reg_val))
		*status = AFVARNL;
	else if (no_os_test_bit(no_os_find_first_set_bit(ADE9153A_AVANL_MSK), &reg_val))
		*status = AVANL;
	else if (no_os_test_bit(no_os_find_first_set_bit(ADE9153A_AWATTNL_MSK),
				&reg_val))
		*status = AWATTNL;
	else
		*status = 0;

	return 0;
}

/**
 * @brief CF2_LT status
 * @param dev - The device structure.
 * @param status - status indicator
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_cf2_lt_status(struct ade9153a_dev *dev, uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_CF_LCFG, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(ADE9153A_CF2_LT_MSK),
				 &reg_val);

	return 0;
}

/**
 * @brief CF1_LT status
 * @param dev - The device structure.
 * @param status - status indicator
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_cf1_lt_status(struct ade9153a_dev *dev, uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_CF_LCFG, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(ADE9153A_CF1_LT_MSK),
				 &reg_val);

	return 0;
}

/**
 * @brief cf_ltmr write value.
 * @param dev - The device structure.
 * @param reg_data - Value to write.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_cf_ltmr_write(struct ade9153a_dev *dev, uint32_t reg_data)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_CF_LCFG,
				    ADE9153A_CF_LTMR_MSK, no_os_field_prep(ADE9153A_CF_LTMR_MSK, reg_data));
}

/**
 * @brief cf_ltmr read value.
 * @param dev - The device structure.
 * @param val - cf_ltmr value
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_cf_ltmr_val(struct ade9153a_dev *dev, uint32_t *val)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!val)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_CF_LCFG, &reg_val);
	if (ret)
		return ret;
	*val = reg_val & (uint32_t)(ADE9153A_CF_LTMR_MSK & 0x07FFFF);

	return 0;
}

/**
 * @brief temperature sensor offset.
 * @param dev - The device structure.
 * @param val - sensor offset value
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_temp_offset_val(struct ade9153a_dev *dev, uint32_t *val)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!val)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_TEMP_TRIM, &reg_val);
	if (ret)
		return ret;
	*val = (uint32_t) ((reg_val >> 16) & ((uint32_t)ADE9153A_TEMP_OFFSET_MSK >>
					      16));

	return 0;
}

/**
 * @brief temperature sensor gain.
 * @param dev - The device structure.
 * @param val - sensor gain value
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_temp_gain_val(struct ade9153a_dev *dev, uint32_t *val)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!val)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_TEMP_TRIM, &reg_val);
	if (ret)
		return ret;
	*val = reg_val & (uint32_t)(ADE9153A_TEMP_GAIN_MSK);

	return 0;
}

/**
 * @brief External voltage reference enable/disable.
 * @param dev - The device structure.
 * @param en - enable / disable val.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_ext_ref(struct ade9153a_dev *dev, uint8_t en)
{
	return ade9153a_update_bits(dev, ADE9153A_REG_CONFIG1, ADE9153A_EXT_REF_MSK,
				    no_os_field_prep(ADE9153A_EXT_REF_MSK, en));
}

/**
 * @brief External_reference status
 * @param dev - The device structure.
 * @param status - status indicator
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_ext_ref_status(struct ade9153a_dev *dev, uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_CONFIG1, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(ADE9153A_EXT_REF_MSK),
				 &reg_val);

	return 0;
}

/**
 * @brief Dip swell interrupt mode.
 * @param dev - The device structure.
 * @param mode - IRQ mode
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_dip_swell_irq_mode(struct ade9153a_dev *dev,
				enum dip_swell_irq_mode_en mode)
{
	int ret;

	if (!dev)
		return -ENODEV;

	switch (mode) {
	case ONE_INT:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG1,
					   ADE9153A_DIP_SWELL_IRQ_MODE_MSK,
					   no_os_field_prep(ADE9153A_DIP_SWELL_IRQ_MODE_MSK, ENABLE));
		break;
	default:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG1,
					   ADE9153A_DIP_SWELL_IRQ_MODE_MSK,
					   no_os_field_prep(ADE9153A_DIP_SWELL_IRQ_MODE_MSK, DISABLE));
		break;
	}

	return ret;
}

/**
 * @brief Burst read enable/disable.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_burst_en(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;
	if (dev->burst_en)
		return ade9153a_update_bits(dev, ADE9153A_REG_CONFIG1, ADE9153A_BURST_EN_MSK,
					    no_os_field_prep(ADE9153A_BURST_EN_MSK, ENABLE));
	else
		return ade9153a_update_bits(dev, ADE9153A_REG_CONFIG1, ADE9153A_BURST_EN_MSK,
					    no_os_field_prep(ADE9153A_BURST_EN_MSK, DISABLE));
}

/**
 * @brief Set PWR_SETTLE
 * @param dev - The device structure.
 * @param time - The time values
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_pwr_settle_set(struct ade9153a_dev *dev,
			    enum ade9153a_pwr_settle_e time)
{
	int ret;

	if (!dev)
		return -ENODEV;

	switch (time) {
	case ADE9153A_SETTLE_0_MS:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG1, ADE9153A_PWR_SETTLE_MSK,
					   no_os_field_prep(ADE9153A_PWR_SETTLE_MSK, ADE9153A_SETTLE_0_MS));
		break;
	case ADE9153A_SETTLE_256_MS:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG1, ADE9153A_PWR_SETTLE_MSK,
					   no_os_field_prep(ADE9153A_PWR_SETTLE_MSK, ADE9153A_SETTLE_256_MS));
		break;
	case ADE9153A_SETTLE_128_MS:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG1, ADE9153A_PWR_SETTLE_MSK,
					   no_os_field_prep(ADE9153A_PWR_SETTLE_MSK, ADE9153A_SETTLE_128_MS));
		break;
	default:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG1, ADE9153A_PWR_SETTLE_MSK,
					   no_os_field_prep(ADE9153A_PWR_SETTLE_MSK, ADE9153A_SETTLE_64_MS));
		break;
	}

	return ret;
}

/**
 * @brief Clear accumulation in the digital freq conv.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_cf_acc_clr(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_CONFIG1, ADE9153A_CF_ACC_CLR_MSK,
				    no_os_field_prep(ADE9153A_CF_ACC_CLR_MSK, ENABLE));
}

/**
 * @brief ZX driven to CF2 pin.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_zx_out_oe(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_CONFIG1, ADE9153A_ZX_OUT_OE_MSK,
				    no_os_field_prep(ADE9153A_ZX_OUT_OE_MSK, ENABLE));
}

/**
 * @brief DREADY driven to CF2 pin.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_dready_oe(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_CONFIG1, ADE9153A_DREADY_OE_MSK,
				    no_os_field_prep(ADE9153A_DREADY_OE_MSK, ENABLE));
}

/**
 * @brief CF2 output disable.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_cf2dis(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_CFMODE, ADE9153A_CF2DIS_MSK,
				    no_os_field_prep(ADE9153A_CF2DIS_MSK, ENABLE));
}

/**
 * @brief CF1 output disable.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_cf1dis(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_CFMODE, ADE9153A_CF1DIS_MSK,
				    no_os_field_prep(ADE9153A_CF1DIS_MSK, ENABLE));
}

/**
 * @brief Set type of energy output on CF2 pin
 * @param dev - The device structure.
 * @param energy_type - type of energy output
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_cf2sel(struct ade9153a_dev *dev,
		    enum ade9153a_cf2sel_e energy_type)
{
	int ret;

	if (!dev)
		return -ENODEV;

	switch (energy_type) {
	case ADE9153A_TOTAL_FUNDAMENTAL_REACTIVE_POWER:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG1, ADE9153A_CF2SEL_MSK,
					   no_os_field_prep(ADE9153A_CF2SEL_MSK,
							   ADE9153A_TOTAL_FUNDAMENTAL_REACTIVE_POWER * 2));
		break;
	case ADE9153A_TOTAL_APPARENT_POWER:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG1, ADE9153A_CF2SEL_MSK,
					   no_os_field_prep(ADE9153A_CF2SEL_MSK, ADE9153A_TOTAL_APPARENT_POWER * 2));
		break;
	default:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG1, ADE9153A_CF2SEL_MSK,
					   no_os_field_prep(ADE9153A_CF2SEL_MSK, ADE9153A_TOTAL_ACTIVE_POWER));
		break;
	}

	return ret;
}

/**
 * @brief Set type of energy output on CF1 pin
 * @param dev - The device structure.
 * @param energy_type - type of energy output
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_cf1sel(struct ade9153a_dev *dev,
		    enum ade9153a_cf2sel_e energy_type)
{
	int ret;

	if (!dev)
		return -ENODEV;

	switch (energy_type) {
	case ADE9153A_TOTAL_FUNDAMENTAL_REACTIVE_POWER:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG1, ADE9153A_CF1SEL_MSK,
					   no_os_field_prep(ADE9153A_CF1SEL_MSK,
							   ADE9153A_TOTAL_FUNDAMENTAL_REACTIVE_POWER * 2));
		break;
	case ADE9153A_TOTAL_APPARENT_POWER:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG1, ADE9153A_CF1SEL_MSK,
					   no_os_field_prep(ADE9153A_CF1SEL_MSK, ADE9153A_TOTAL_APPARENT_POWER * 2));
		break;
	default:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG1, ADE9153A_CF1SEL_MSK,
					   no_os_field_prep(ADE9153A_CF1SEL_MSK, ADE9153A_TOTAL_ACTIVE_POWER));
		break;
	}

	return ret;
}

/**
 * @brief Frequency select.
 * @param dev - The device structure.
 * @param freq - 0 - 50 Hz, 1 - 60 Hz
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_selfreq(struct ade9153a_dev *dev, enum ade9153a_selfreq_e freq)
{
	int ret;

	if (!dev)
		return -ENODEV;

	switch (freq) {
	case F_60_HZ:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_ACCMODE, ADE9153A_SELFREQ_MSK,
					   no_os_field_prep(ADE9153A_SELFREQ_MSK, F_60_HZ));
		break;
	default:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_ACCMODE, ADE9153A_SELFREQ_MSK,
					   no_os_field_prep(ADE9153A_SELFREQ_MSK, F_50_HZ));
	}

	return ret;
}

/**
 * @brief Frequency setting
 * @param dev - The device structure.
 * @param status - status 0 - 50Hz 1 - 60Hz
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_freq_s_status(struct ade9153a_dev *dev, uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_ACCMODE, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(ADE9153A_SELFREQ_MSK),
				 &reg_val);

	return 0;
}

/**
 * @brief Select fundamental reactive power accumulation mode
 * @param dev - The device structure.
 * @param varacc_type - type of energy output
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_varacc(struct ade9153a_dev *dev,
		    enum ade9153a_accmode_e varacc_type)
{
	int ret;

	if (!dev)
		return -ENODEV;

	switch (varacc_type) {
	case ADE9153A_NEGATIVE_ACC_MODE:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_ACCMODE, ADE9153A_VARACC_MSK,
					   no_os_field_prep(ADE9153A_VARACC_MSK, ADE9153A_NEGATIVE_ACC_MODE));
		break;
	case ADE9153A_POSITIVE_ACC_MODE:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_ACCMODE, ADE9153A_VARACC_MSK,
					   no_os_field_prep(ADE9153A_VARACC_MSK, ADE9153A_POSITIVE_ACC_MODE));
		break;
	case ADE9153A_ABSOLUTE_VAL_ACC_MODE:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_ACCMODE, ADE9153A_VARACC_MSK,
					   no_os_field_prep(ADE9153A_VARACC_MSK, ADE9153A_ABSOLUTE_VAL_ACC_MODE));
		break;
	default:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_ACCMODE, ADE9153A_VARACC_MSK,
					   no_os_field_prep(ADE9153A_VARACC_MSK, ADE9153A_SIGNED_ACC_MODE));
		break;
	}

	return ret;
}

/**
 * @brief Select Total active power accumulation mode
 * @param dev - The device structure.
 * @param wattacc_type - type of energy output
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_wattacc(struct ade9153a_dev *dev,
		     enum ade9153a_accmode_e wattacc_type)
{
	int ret;

	if (!dev)
		return -ENODEV;

	switch (wattacc_type) {
	case ADE9153A_NEGATIVE_ACC_MODE:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_ACCMODE, ADE9153A_WATTACC_MSK,
					   no_os_field_prep(ADE9153A_WATTACC_MSK, ADE9153A_NEGATIVE_ACC_MODE));
		break;
	case ADE9153A_POSITIVE_ACC_MODE:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_ACCMODE, ADE9153A_WATTACC_MSK,
					   no_os_field_prep(ADE9153A_WATTACC_MSK, ADE9153A_POSITIVE_ACC_MODE));
		break;
	case ADE9153A_ABSOLUTE_VAL_ACC_MODE:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_ACCMODE, ADE9153A_WATTACC_MSK,
					   no_os_field_prep(ADE9153A_WATTACC_MSK, ADE9153A_ABSOLUTE_VAL_ACC_MODE));
		break;
	default:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_ACCMODE, ADE9153A_WATTACC_MSK,
					   no_os_field_prep(ADE9153A_WATTACC_MSK, ADE9153A_SIGNED_ACC_MODE));
		break;
	}

	return ret;
}

/**
 * @brief Select peak detection channels
 * @param dev - The device structure.
 * @param peak_sel - Channel selection for peak detection
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_peak_sel(struct ade9153a_dev *dev,
		      enum ade9153a_peak_sel_e peak_sel)
{
	int ret;

	if (!dev)
		return -ENODEV;

	switch (peak_sel) {
	case ADE9153A_PEAK_DETECTION_ENABLE_V_I_PHA_PHB:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG3, ADE9153A_PEAK_SEL_MSK,
					   no_os_field_prep(ADE9153A_PEAK_SEL_MSK,
							   ADE9153A_PEAK_DETECTION_ENABLE_V_I_PHA_PHB));
		break;
	case ADE9153A_PEAK_DETECTION_ENABLE_I_PHB:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG3, ADE9153A_PEAK_SEL_MSK,
					   no_os_field_prep(ADE9153A_PEAK_SEL_MSK, ADE9153A_PEAK_DETECTION_ENABLE_I_PHB));
		break;
	case ADE9153A_PEAK_DETECTION_ENABLE_V_I_PHA:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG3, ADE9153A_PEAK_SEL_MSK,
					   no_os_field_prep(ADE9153A_PEAK_SEL_MSK,
							   ADE9153A_PEAK_DETECTION_ENABLE_V_I_PHA));
		break;
	default:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG3, ADE9153A_PEAK_SEL_MSK,
					   no_os_field_prep(ADE9153A_PEAK_SEL_MSK,
							   ADE9153A_PEAK_DETECTION_DISABLE_PHA_PHB));
		break;
	}

	return ret;
}

/**
 * @brief Ch B overcurretn detection enable.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_oib_en(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_CONFIG3, ADE9153A_OIB_EN_MSK,
				    no_os_field_prep(ADE9153A_OIB_EN_MSK, ENABLE));
}

/**
 * @brief Ch A overcurretn detection enable.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_oia_en(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_CONFIG3, ADE9153A_OIA_EN_MSK,
				    no_os_field_prep(ADE9153A_OIA_EN_MSK, ENABLE));
}

/**
 * @brief Zero crossing lpf disable.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_diszxlpf(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_ZX_CFG, ADE9153A_DISZXLPF_MSK,
				    no_os_field_prep(ADE9153A_DISZXLPF_MSK, ENABLE));
}

/**
 * @brief Sign of CF2 power
 * @param dev - The device structure.
 * @param status - status indicator 0 - positive 1 - negative
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_cf2sign_status(struct ade9153a_dev *dev, uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_PHSIGN, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(ADE9153A_CF2SIGN_MSK),
				 &reg_val);

	return 0;
}

/**
 * @brief Sign of CF1 power
 * @param dev - The device structure.
 * @param status - status indicator 0 - positive 1 - negative
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_cf1sign_status(struct ade9153a_dev *dev, uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_PHSIGN, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(ADE9153A_CF1SIGN_MSK),
				 &reg_val);

	return 0;
}

/**
 * @brief Sign of reactive power
 * @param dev - The device structure.
 * @param status - status indicator 0 - positive 1 - negative
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_avarsign_status(struct ade9153a_dev *dev, uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_PHSIGN, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(ADE9153A_AVARSIGN_MSK),
				 &reg_val);

	return 0;
}

/**
 * @brief Sign of active power
 * @param dev - The device structure.
 * @param status - status indicator 0 - positive 1 - negative
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_awsign_status(struct ade9153a_dev *dev, uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_PHSIGN, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(ADE9153A_AWSIGN_MSK),
				 &reg_val);

	return 0;
}

/**
 * @brief User period select.
 * @param dev - The device structure.
 * @param en - 0 -voltage line period 1 - user configured line period
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_uperiod_sel(struct ade9153a_dev *dev, uint8_t en)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_CONFIG2, ADE9153A_UPERIOD_SEL_MSK,
				    no_os_field_prep(ADE9153A_UPERIOD_SEL_MSK, en));
}

/**
 * @brief Hpf corner freq
 * @param dev - The device structure.
 * @param hpf_corner_freq - the corner freq for hpf
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_hpf_crn(struct ade9153a_dev *dev,
		     enum ade9153a_hpf_crn_e hpf_corner_freq)
{
	int ret;

	if (!dev)
		return -ENODEV;

	switch (hpf_corner_freq) {
	case ADE9153A_HPF_CORNER_0_3125_HZ:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG2, ADE9153A_HPF_CRN_MSK,
					   no_os_field_prep(ADE9153A_HPF_CRN_MSK, ADE9153A_HPF_CORNER_0_3125_HZ));
		break;
	case ADE9153A_HPF_CORNER_0_625_HZ:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG2, ADE9153A_HPF_CRN_MSK,
					   no_os_field_prep(ADE9153A_HPF_CRN_MSK, ADE9153A_HPF_CORNER_0_625_HZ));
		break;
	case ADE9153A_HPF_CORNER_1_2475_HZ:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG2, ADE9153A_HPF_CRN_MSK,
					   no_os_field_prep(ADE9153A_HPF_CRN_MSK, ADE9153A_HPF_CORNER_1_2475_HZ));
		break;
	case ADE9153A_HPF_CORNER_2_49_HZ:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG2, ADE9153A_HPF_CRN_MSK,
					   no_os_field_prep(ADE9153A_HPF_CRN_MSK, ADE9153A_HPF_CORNER_2_49_HZ));
		break;
	case ADE9153A_HPF_CORNER_4_9675_HZ:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG2, ADE9153A_HPF_CRN_MSK,
					   no_os_field_prep(ADE9153A_HPF_CRN_MSK, ADE9153A_HPF_CORNER_4_9675_HZ));
		break;
	case ADE9153A_HPF_CORNER_9_895_HZ:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG2, ADE9153A_HPF_CRN_MSK,
					   no_os_field_prep(ADE9153A_HPF_CRN_MSK, ADE9153A_HPF_CORNER_9_895_HZ));
		break;
	case ADE9153A_HPF_CORNER_19_6375_HZ:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG2, ADE9153A_HPF_CRN_MSK,
					   no_os_field_prep(ADE9153A_HPF_CRN_MSK, ADE9153A_HPF_CORNER_19_6375_HZ));
		break;
	default:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_CONFIG2, ADE9153A_HPF_CRN_MSK,
					   no_os_field_prep(ADE9153A_HPF_CRN_MSK, ADE9153A_HPF_CORNER_38_695_HZ));
		break;
	}

	return ret;
}

/**
 * @brief Set no. of samples for no load condition
 * @param dev - The device structure.
 * @param noload_samples - no. of samples
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_noload_tmr(struct ade9153a_dev *dev,
			enum ade9153a_noload_tmr_e noload_samples)
{
	int ret;

	if (!dev)
		return -ENODEV;

	switch (noload_samples) {
	case ADE9153A_NOLOAD_TMR_DISABLE:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_EP_CFG, ADE9153A_NOLOAD_TMR_MSK,
					   no_os_field_prep(ADE9153A_NOLOAD_TMR_MSK, ADE9153A_NOLOAD_TMR_DISABLE));
		break;
	case ADE9153A_NOLOAD_TMR_SAMPLES_4096:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_EP_CFG, ADE9153A_NOLOAD_TMR_MSK,
					   no_os_field_prep(ADE9153A_NOLOAD_TMR_MSK, ADE9153A_NOLOAD_TMR_SAMPLES_4096));
		break;
	case ADE9153A_NOLOAD_TMR_SAMPLES_2048:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_EP_CFG, ADE9153A_NOLOAD_TMR_MSK,
					   no_os_field_prep(ADE9153A_NOLOAD_TMR_MSK, ADE9153A_NOLOAD_TMR_SAMPLES_2048));
		break;
	case ADE9153A_NOLOAD_TMR_SAMPLES_1024:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_EP_CFG, ADE9153A_NOLOAD_TMR_MSK,
					   no_os_field_prep(ADE9153A_NOLOAD_TMR_MSK, ADE9153A_NOLOAD_TMR_SAMPLES_1024));
		break;
	case ADE9153A_NOLOAD_TMR_SAMPLES_512:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_EP_CFG, ADE9153A_NOLOAD_TMR_MSK,
					   no_os_field_prep(ADE9153A_NOLOAD_TMR_MSK, ADE9153A_NOLOAD_TMR_SAMPLES_512));
		break;
	case ADE9153A_NOLOAD_TMR_SAMPLES_256:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_EP_CFG, ADE9153A_NOLOAD_TMR_MSK,
					   no_os_field_prep(ADE9153A_NOLOAD_TMR_MSK, ADE9153A_NOLOAD_TMR_SAMPLES_256));
		break;
	case ADE9153A_NOLOAD_TMR_SAMPLES_128:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_EP_CFG, ADE9153A_NOLOAD_TMR_MSK,
					   no_os_field_prep(ADE9153A_NOLOAD_TMR_MSK, ADE9153A_NOLOAD_TMR_SAMPLES_128));
		break;
	default:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_EP_CFG, ADE9153A_NOLOAD_TMR_MSK,
					   no_os_field_prep(ADE9153A_NOLOAD_TMR_MSK, ADE9153A_NOLOAD_TMR_SAMPLES_64));
		break;
	}

	return ret;
}

/**
 * @brief Energy register read with value reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_rd_rst_en(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_EP_CFG, ADE9153A_RD_RST_EN_MSK,
				    no_os_field_prep(ADE9153A_RD_RST_EN_MSK, ENABLE));
}

/**
 * @brief Internal energy register accum.
 * @param dev - The device structure.
 * @param en - 0 - internal energy register added to the user accessible energy register.
 * 			   1 - internal energy register overwrites the user accessible energy register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_egy_ld_accum(struct ade9153a_dev *dev, uint8_t en)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_EP_CFG, ADE9153A_EGY_LD_ACCUM_MSK,
				    no_os_field_prep(ADE9153A_EGY_LD_ACCUM_MSK, en));
}

/**
 * @brief Energy accumulated based on the number of 4 kSPS samples or zero-crossing events
 * @param dev - The device structure.
 * @param en - 0 - Accumulate energy based on 4 kSPS samples.
 * 			   1 - Accumulate energy based on the zero-crossing events
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_egy_tmr_mode(struct ade9153a_dev *dev, uint8_t en)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_EP_CFG, ADE9153A_EGY_TMR_MODE_MSK,
				    no_os_field_prep(ADE9153A_EGY_TMR_MODE_MSK, en));
}

/**
 * @brief Enable the energy and power accumulator when the run bit is also set.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_egy_pwr_en(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_EP_CFG, ADE9153A_EGY_PWR_EN_MSK,
				    no_os_field_prep(ADE9153A_EGY_PWR_EN_MSK, ENABLE));
}

/**
 * @brief Force CRC calculation to start (update).
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_force_crc_update(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_CRC_FORCE,
				    ADE9153A_FORCE_CRC_UPDATE_MSK, no_os_field_prep(ADE9153A_FORCE_CRC_UPDATE_MSK,
						    ENABLE));
}

/**
 * @brief Manually request a new temperature sensor reading.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_temp_start(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_TEMP_CFG, ADE9153A_TEMP_START_MSK,
				    no_os_field_prep(ADE9153A_TEMP_START_MSK, ENABLE));
}

/**
 * @brief Enable the temperature sensor.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_temp_en(struct ade9153a_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_TEMP_CFG, ADE9153A_TEMP_EN_MSK,
				    no_os_field_prep(ADE9153A_TEMP_EN_MSK, ENABLE));
}

/**
 * @brief Select the number of temperature readings to average.
 * @param dev - The device structure.
 * @param temp_no_samples - no. of samples
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_temp_time(struct ade9153a_dev *dev,
		       enum ade9153a_temp_time_e temp_no_samples)
{
	int ret;

	if (!dev)
		return -ENODEV;

	switch (temp_no_samples) {
	case ADE9153A_TEMP_TIME_SAMPLES_1024:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_TEMP_CFG, ADE9153A_TEMP_TIME_MSK,
					   no_os_field_prep(ADE9153A_TEMP_TIME_MSK, ADE9153A_TEMP_TIME_SAMPLES_1024));
		break;
	case ADE9153A_TEMP_TIME_SAMPLES_512:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_TEMP_CFG, ADE9153A_TEMP_TIME_MSK,
					   no_os_field_prep(ADE9153A_TEMP_TIME_MSK, ADE9153A_TEMP_TIME_SAMPLES_512));
		break;
	case ADE9153A_TEMP_TIME_SAMPLES_256:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_TEMP_CFG, ADE9153A_TEMP_TIME_MSK,
					   no_os_field_prep(ADE9153A_TEMP_TIME_MSK, ADE9153A_TEMP_TIME_SAMPLES_256));
		break;
	default:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_TEMP_CFG, ADE9153A_TEMP_TIME_MSK,
					   no_os_field_prep(ADE9153A_TEMP_TIME_MSK, ADE9153A_TEMP_TIME_SAMPLES_1));
		break;
	}

	return ret;
}

/**
 * @brief Temperature result
 * @param dev - The device structure.
 * @param val - temp val
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_temp_result(struct ade9153a_dev *dev, uint16_t *val)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!val)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_TEMP_RSLT, &reg_val);
	if (ret)
		return ret;
	*val = (uint16_t)reg_val & (uint16_t)ADE9153A_TEMP_RESULT_MSK;

	return 0;
}

/**
 * @brief COMPMODE.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_compmode(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	return ade9153a_write(dev, ADE9153A_REG_COMPMODE, ADE9153A_COMPMODEVAL);
}

/**
 * @brief Start measurements.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_run(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	return ade9153a_write(dev, ADE9153A_REG_RUN, ADE9153A_RUN);
}

/**
 * @brief Stop measurements.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_stop(struct ade9153a_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	return ade9153a_write(dev, ADE9153A_REG_RUN, ADE9153A_STOP);
}

/**
 * @brief Set the signal side of the PGA.
 * @param dev - The device structure.
 * @param set - 0 - signal on IAN 1 - signal on IAP
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_ai_swap(struct ade9153a_dev *dev, uint8_t set)
{
	if (!dev)
		return -ENODEV;

	return ade9153a_update_bits(dev, ADE9153A_REG_AI_PGAGAIN, ADE9153A_AI_SWAP_MSK,
				    no_os_field_prep(ADE9153A_AI_SWAP_MSK, set));
}

/**
 * @brief Set the PGA gain for I chA.
 * @param dev - The device structure.
 * @param gain - Gain value
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_ai_gain(struct ade9153a_dev *dev, enum ade9153a_ai_gain_e gain)
{
	int ret;

	if (!dev)
		return -ENODEV;

	switch (gain) {
	case ADE9153A_AI_GAIN_38_4:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_AI_PGAGAIN, ADE9153A_AI_GAIN_MSK,
					   no_os_field_prep(ADE9153A_AI_GAIN_MSK, ADE9153A_AI_GAIN_38_4));
		break;
	case ADE9153A_AI_GAIN_32:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_AI_PGAGAIN, ADE9153A_AI_GAIN_MSK,
					   no_os_field_prep(ADE9153A_AI_GAIN_MSK, ADE9153A_AI_GAIN_32));
		break;
	case ADE9153A_AI_GAIN_24:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_AI_PGAGAIN, ADE9153A_AI_GAIN_MSK,
					   no_os_field_prep(ADE9153A_AI_GAIN_MSK, ADE9153A_AI_GAIN_24));
		break;
	default:
		ret = ade9153a_update_bits(dev, ADE9153A_REG_AI_PGAGAIN, ADE9153A_AI_GAIN_MSK,
					   no_os_field_prep(ADE9153A_AI_GAIN_MSK, ADE9153A_AI_GAIN_16));
		break;
	}

	return ret;
}

/**
 * @brief New run of mSure ready
 * @param dev - The device structure.
 * @param status - status indicator 1 - ready
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_ms_sysrdy(struct ade9153a_dev *dev, uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_MS_STATUS_IRQ, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(ADE9153A_MS_SYSRDY_MSK),
				 &reg_val);

	return 0;
}

/**
 * @brief Invalid config of mSure status
 * @param dev - The device structure.
 * @param status - status indicator 1 - config error
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_ms_conferr(struct ade9153a_dev *dev, uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_MS_STATUS_IRQ, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(ADE9153A_MS_CONFERR_MSK),
				 &reg_val);

	return 0;
}

/**
 * @brief mSure not detected on the last enabled ch
 * @param dev - The device structure.
 * @param status - status indicator 1 - mSure not detected
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_ms_absent(struct ade9153a_dev *dev, uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_MS_STATUS_IRQ, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(ADE9153A_MS_ABSENT_MSK),
				 &reg_val);

	return 0;
}

/**
 * @brief mSure timed out (600s)
 * @param dev - The device structure.
 * @param status - status indicator 1 - time out
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_ms_timeout(struct ade9153a_dev *dev, uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_MS_STATUS_IRQ, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(ADE9153A_MS_TIMEOUT_MSK),
				 &reg_val);

	return 0;
}

/**
 * @brief mSure ready
 * @param dev - The device structure.
 * @param status - status indicator 1 - ready
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_ms_ready(struct ade9153a_dev *dev, uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_MS_STATUS_IRQ, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(ADE9153A_MS_READY_MSK),
				 &reg_val);

	return 0;
}

/**
 * @brief mSure shift detected in CC val
 * @param dev - The device structure.
 * @param status - status indicator 1 - shift detected (must run mSure again)
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_ms_shift(struct ade9153a_dev *dev, uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_MS_STATUS_IRQ, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(ADE9153A_MS_SHIFT_MSK),
				 &reg_val);

	return 0;
}

/**
 * @brief Chip status
 * @param dev - The device structure.
 * @param status - Status code
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_chip_status(struct ade9153a_dev *dev, uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9153a_read(dev, ADE9153A_REG_MS_STATUS_IRQ, &reg_val);
	if (ret)
		return ret;

	if (no_os_test_bit(no_os_find_first_set_bit(ADE9153A_UART_RESET_MSK),
			   &reg_val) )
		*status = ADE9153A_UART_RESET;
	else if (no_os_test_bit(no_os_find_first_set_bit(ADE9153A_UART_ERROR2_MSK),
				&reg_val))
		*status = ADE9153A_UART_ERROR2;
	else if (no_os_test_bit(no_os_find_first_set_bit(ADE9153A_UART_ERROR1_MSK),
				&reg_val))
		*status = ADE9153A_UART_ERROR1;
	else if (no_os_test_bit(no_os_find_first_set_bit(ADE9153A_UART_ERROR0_MSK),
				&reg_val))
		*status = ADE9153A_UART_ERROR0;
	else if (no_os_test_bit(no_os_find_first_set_bit(ADE9153A_ERROR3_MSK),
				&reg_val))
		*status = ADE9153A_ERROR3;
	else if (no_os_test_bit(no_os_find_first_set_bit(ADE9153A_ERROR2_MSK),
				&reg_val))
		*status = ADE9153A_ERROR2;
	else if (no_os_test_bit(no_os_find_first_set_bit(ADE9153A_ERROR1_MSK),
				&reg_val))
		*status = ADE9153A_ERROR1;
	else if (no_os_test_bit(no_os_find_first_set_bit(ADE9153A_ERROR0_MSK),
				&reg_val))
		*status = ADE9153A_ERROR0;

	return 0;
}

/**
 * @brief Temperature value in deg C
 * @param dev - The device structure.
 * @param data - Structure to store temperature values
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_temp_val(struct ade9153a_dev *dev,
		      struct ade9153a_temperature_value *data)
{
	int ret;
	/* temperature offset value read*/
	uint32_t offset;
	/* temperature gain value read*/
	uint32_t gain;
	/* temperature value read */
	uint32_t temp;
	/* status of temperature ready */
	uint8_t status = 0;
	/* timeout for temperature read */
	uint16_t timeout = 0;

	if (!dev)
		return -ENODEV;
	if (!data)
		return -EINVAL;

	// read temperature offset
	ret = ade9153a_temp_offset_val(dev, &offset);
	if (ret)
		return ret;
	data->offset_reg_val = (uint16_t)offset;

	// read temperature gain
	ret = ade9153a_temp_gain_val(dev, &gain);
	if (ret)
		return ret;
	data->gain_reg_val = (uint16_t)gain;

	// enable temperature sensor
	ret = ade9153a_temp_en(dev);
	if (ret)
		return ret;

	// clear temperature ready flag
	ret = ade9153a_clear_temp_rdy(dev);
	if (ret)
		return ret;

	// start a temperature conversion
	ret = ade9153a_temp_start(dev);
	if (ret)
		return ret;

	// wait for conversion result
	while(!(status)) {
		ret = ade9153a_get_temp_rdy(dev, &status);
		no_os_mdelay(1);
		timeout++;
		if (timeout == 2000) {
			ret = -ENODATA;
			return ret;
		}
	}

	// if conversion successfuly read temperature value
	ret = ade9153a_temp_result(dev, &temp);
	if (ret)
		return ret;
	data->temperature_reg_val = (uint16_t)temp;

	return 0;
}

/**
 * @brief Read energy values
 * @param dev - The device structure.
 * @param data - Structure to store energy values
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_energy_vals(struct ade9153a_dev *dev,
			 struct ade9153a_energy_values *data)
{
	int ret;
	/* temporary value read from register */
	uint32_t temp_val;

	if (!dev)
		return -ENODEV;
	if (!data)
		return -EINVAL;

	// read accumulated total active energy MSBs
	ret = ade9153a_read(dev, ADE9153A_REG_AWATTHR_HI, &temp_val);
	if (ret)
		return ret;
	data->active_energy_reg_val = (int32_t)temp_val;

	// read accumulated fundamental reactive energy MSBs
	ret = ade9153a_read(dev, ADE9153A_REG_AFVARHR_HI, &temp_val);
	if (ret)
		return ret;
	data->fundamental_reactive_energy_reg_val = (int32_t)temp_val;

	// read accumulated total apparent energy MSBs
	ret = ade9153a_read(dev, ADE9153A_REG_AVAHR_HI, &temp_val);
	if (ret)
		return ret;
	data->apparent_energy_reg_val = (int32_t)temp_val;

	return 0;
}

/**
 * @brief Read power values
 * @param dev - The device structure.
 * @param data - Structure to store power values
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_power_vals(struct ade9153a_dev *dev,
			struct ade9153a_power_values *data)
{
	int ret;
	/* temporary value read from register */
	uint32_t temp_val;

	if (!dev)
		return -ENODEV;
	if (!data)
		return -EINVAL;

	// read total active power
	ret = ade9153a_read(dev, ADE9153A_REG_AWATT, &temp_val);
	if (ret)
		return ret;
	data->active_power_reg_val = (int32_t)temp_val;

	// read fundamental reactive power
	ret = ade9153a_read(dev, ADE9153A_REG_AFVAR, &temp_val);
	if (ret)
		return ret;
	data->fundamental_reactive_power_reg_val = (int32_t)temp_val;

	// read total apparent power
	ret = ade9153a_read(dev, ADE9153A_REG_AVA, &temp_val);
	if (ret)
		return ret;
	data->apparent_power_reg_val = temp_val;

	return 0;
}

/**
 * @brief Read rms values
 * @param dev - The device structure.
 * @param data - Structure to store rms values
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_rms_vals(struct ade9153a_dev *dev,
		      struct ade9153a_rms_values *data)
{
	int ret;
	/* temporary value read from register */
	uint32_t temp_val;

	if (!dev)
		return -ENODEV;
	if (!data)
		return -EINVAL;

	// read current rms value
	ret = ade9153a_read(dev, ADE9153A_REG_AIRMS, &temp_val);
	if (ret)
		return ret;
	data->current_rms_reg_val = (int32_t)temp_val;

	// read voltage rms value
	ret = ade9153a_read(dev, ADE9153A_REG_AVRMS, &temp_val);
	if (ret)
		return ret;
	data->voltage_rms_reg_val = (int32_t)temp_val;

	return 0;
}

/**
 * @brief Read half rms values
 * @param dev - The device structure.
 * @param data - Structure to store half rms values
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_half_rms_vals(struct ade9153a_dev *dev,
			   struct ade9153a_half_rms_values *data)
{
	int ret;
	/* temporary value read from register */
	uint32_t temp_val;

	if (!dev)
		return -ENODEV;
	if (!data)
		return -EINVAL;

	// read current rms value
	ret = ade9153a_read(dev, ADE9153A_REG_AIRMS_OC, &temp_val);
	if (ret)
		return ret;
	data->current_h_rms_reg_val = (int32_t)temp_val;

	// read voltage rms value
	ret = ade9153a_read(dev, ADE9153A_REG_AVRMS_OC, &temp_val);
	if (ret)
		return ret;
	data->voltage_h_rms_reg_val = (int32_t)temp_val;

	return 0;
}

/**
 * @brief Read power quaility values
 * @param dev - The device structure.
 * @param data - Structure to store power quality values
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_power_quality_vals(struct ade9153a_dev *dev,
				struct ade9153a_pq_values *data)
{
	int ret;
	/* temporary value read from register */
	uint32_t temp_val;

	if (!dev)
		return -ENODEV;
	if (!data)
		return -EINVAL;

	// read power factor value
	ret = ade9153a_read(dev, ADE9153A_REG_APF, &temp_val);
	if (ret)
		return ret;
	data->power_factor_reg_val = (int32_t)temp_val;

	// read voltage rms value
	ret = ade9153a_read(dev, ADE9153A_REG_APERIOD, &temp_val);
	if (ret)
		return ret;
	data->period_reg_val = (int32_t)temp_val;

	// read angle value
	ret = ade9153a_read(dev, ADE9153A_REG_ANGL_AV_AI, &temp_val);
	if (ret)
		return ret;
	data->angle_ai_av_reg_val = (int16_t)temp_val;

	return 0;
}

/**
 * @brief Start autocalibration AI channel
 * @param dev - The device structure.
 * @param mode - normal or turbo
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_start_autocal_ai(struct ade9153a_dev *dev,
			      enum ade9153a_acalmode_e mode)
{
	int ret;
	/* status of the calibration engine */
	uint8_t status;

	if (!dev)
		return -ENODEV;

	ret = ade9153a_msure_status(dev, &status);
	if (ret)
		return ret;
	if (status) {
		ret = ade9153a_auto_calibration_cfg(dev, AUTOCAL_AI, mode);
		if (ret)
			return ret;
		ret = ade9153a_auto_calibration_run(dev);
		if (ret)
			return ret;
	} else
		return -EBUSY;

	return 0;
}

/**
 * @brief Start autocalibration BI channel
 * @param dev - The device structure.
 * @param mode - normal or turbo
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_start_autocal_bi(struct ade9153a_dev *dev,
			      enum ade9153a_acalmode_e mode)
{
	int ret;
	/* status of the calibration engine */
	uint8_t status;

	if (!dev)
		return -ENODEV;

	ret = ade9153a_msure_status(dev, &status);
	if (ret)
		return ret;
	if (status) {
		ret = ade9153a_auto_calibration_cfg(dev, AUTOCAL_BI, mode);
		if (ret)
			return ret;
		ret = ade9153a_auto_calibration_run(dev);
		if (ret)
			return ret;
	} else
		return -EBUSY;

	return 0;
}

/**
 * @brief Start autocalibration AV channel
 * @param dev - The device structure.
 * @param mode - normal or turbo
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_start_autocal_av(struct ade9153a_dev *dev,
			      enum ade9153a_acalmode_e mode)
{
	int ret;
	/* status of the calibration engine */
	uint8_t status;

	if (!dev)
		return -ENODEV;

	ret = ade9153a_msure_status(dev, &status);
	if (ret)
		return ret;
	if (status) {
		ret = ade9153a_auto_calibration_cfg(dev, AUTOCAL_AV, mode);
		if (ret)
			return ret;
		ret = ade9153a_auto_calibration_run(dev);
		if (ret)
			return ret;
	} else
		return -EBUSY;

	return 0;
}

/**
 * @brief Read autocalibration values
 * @param dev - The device structure.
 * @param data - Structure to store autocalibration values
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9153a_read_autocal_vals(struct ade9153a_dev *dev,
			       struct ade9153a_autocal_vals *data)
{
	int ret;
	/* temporary value read from register */
	uint32_t temp_val;

	if (!dev)
		return -ENODEV;

	// read aicc value
	ret = ade9153a_read(dev, ADE9153A_REG_MS_ACAL_AICC, &temp_val);
	if (ret)
		return ret;
	data->aicc_reg_val = (int32_t)temp_val;

	// read aicert value
	ret = ade9153a_read(dev, ADE9153A_REG_MS_ACAL_AICERT, &temp_val);
	if (ret)
		return ret;
	data->aicert_reg_val = (int32_t)temp_val;

	// read avcc value
	ret = ade9153a_read(dev, ADE9153A_REG_MS_ACAL_AVCC, &temp_val);
	if (ret)
		return ret;
	data->avcc_reg_val = (int32_t)temp_val;

	// read avcert value
	ret = ade9153a_read(dev, ADE9153A_REG_MS_ACAL_AVCERT, &temp_val);
	if (ret)
		return ret;
	data->avcert_reg_val = (int32_t)temp_val;

	return 0;
}
