/***************************************************************************//**
 *   @file   ad4080.c
 *   @brief  Implementation of AD4080 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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

#include <stdlib.h>
#include <errno.h>
#include "ad4080.h"
#include "no_os_delay.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/******************************************************************************/

/**
 * @brief Write device register.
 * @param dev- The device structure.
 * @param reg_addr - The register address.
 * @param reg_val - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_write(struct ad4080_dev *dev, uint16_t reg_addr, uint8_t reg_val)
{
	uint8_t buff[3];

	if (!dev)
		return -EINVAL;

	buff[0] = no_os_field_get(BYTE_ADDR_H, reg_addr);
	buff[1] = no_os_field_get(BYTE_ADDR_L, reg_addr);
	buff[2] = reg_val;

	return no_os_spi_write_and_read(dev->spi_desc, buff, 3);
}

/**
 * @brief Read device register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_val - The data read from the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_read(struct ad4080_dev *dev, uint16_t reg_addr, uint8_t *reg_val)
{
	int32_t ret;
	uint8_t buff[3];

	if (!dev)
		return -EINVAL;

	buff[0] = no_os_field_get(BYTE_ADDR_H, reg_addr) | AD4080_SPI_READ;
	buff[1] = no_os_field_get(BYTE_ADDR_L, reg_addr);

	ret = no_os_spi_write_and_read(dev->spi_desc, buff, 3);
	if (ret)
		return ret;

	*reg_val = buff[2];

	return 0;
}

/**
 * @brief Update specific register bits.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Specific bits mask.
 * @param reg_val - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_update_bits(struct ad4080_dev *dev, uint16_t reg_addr, uint8_t mask,
		       uint8_t reg_val)
{
	int ret;
	uint8_t data;

	if (!dev)
		return -EINVAL;

	ret = ad4080_read(dev, reg_addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_val;

	return ad4080_write(dev, reg_addr, data);
}

/**
 * @brief Software reset the device.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_soft_reset(struct ad4080_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4080_update_bits(dev, AD4080_REG_INTERFACE_CONFIG_A,
				 AD4080_SW_RESET_MSK, AD4080_SW_RESET);
	if (ret)
		return ret;

	return ad4080_update_bits(dev, AD4080_REG_INTERFACE_CONFIG_A,
				  AD4080_SW_RESET_MSK, 0);
}

/**
 * @brief Set Address Ascension
 * @param dev - The device structure.
 * @param addr_asc - The device mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_set_addr_asc(struct ad4080_dev *dev, enum ad4080_addr_asc addr_asc)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4080_update_bits(dev, AD4080_REG_INTERFACE_CONFIG_A,
				 AD4080_ADDR_ASC_MSK,
				 no_os_field_prep(AD4080_ADDR_ASC_MSK, addr_asc));
	if (ret)
		return ret;

	dev->addr_asc = addr_asc;

	return 0;
}

/**
 * @brief Get Address Ascension
 * @param dev - The device structure.
 * @return Address ascension value
 */
enum ad4080_addr_asc ad4080_get_addr_asc(struct ad4080_dev *dev)
{
	return dev->addr_asc;
}

/**
 * @brief Set Single/Streaming Mode
 * @param dev - The device structure.
 * @param single_instr - The device mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_set_single_instr(struct ad4080_dev *dev,
			    enum ad4080_single_instr single_instr)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4080_update_bits(dev, AD4080_REG_INTERFACE_CONFIG_B,
				 AD4080_SINGLE_INST_MSK,
				 no_os_field_prep(AD4080_SINGLE_INST_MSK, single_instr));
	if (ret)
		return ret;

	dev->single_instr = single_instr;

	return 0;
}

/**
 * @brief Get Single/Streaming Mode
 * @param dev - The device structure.
 * @return Single/Streaming Mode value
 */
enum ad4080_single_instr ad4080_get_single_instr(struct ad4080_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->single_instr;
}

/**
 * @brief Set Short Instruction
 * @param dev - The device structure.
 * @param short_instr - The device mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_set_short_instr(struct ad4080_dev *dev,
			   enum ad4080_short_instr short_instr)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4080_update_bits(dev, AD4080_REG_INTERFACE_CONFIG_B,
				 AD4080_SHORT_INST_MSK,
				 no_os_field_prep(AD4080_SHORT_INST_MSK, short_instr));
	if (ret)
		return ret;

	dev->short_instr = short_instr;

	return 0;
}

/**
 * @brief Get Short Instruction
 * @param dev - The device structure.
 * @return Short Instruction value
 */
enum ad4080_short_instr ad4080_get_short_instr(struct ad4080_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->short_instr;
}

/**
 * @brief Set Strict Register Access
 * @param dev - The device structure.
 * @param strict_reg - The device mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_set_strict_reg_access(struct ad4080_dev *dev,
				 enum ad4080_strict_reg_access strict_reg)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4080_update_bits(dev, AD4080_REG_INTERFACE_CONFIG_C,
				 AD4080_STRICT_REG_ACCESS_MSK,
				 no_os_field_prep(AD4080_STRICT_REG_ACCESS_MSK, strict_reg));
	if (ret)
		return ret;

	dev->strict_reg = strict_reg;

	return 0;
}

/**
 * @brief Get Strict Register Access
 * @param dev - The device structure.
 * @return Strict Register Access value
 */
enum ad4080_strict_reg_access ad4080_get_strict_reg_access(
	struct ad4080_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->strict_reg;
}

/**
 * @brief Set AD4080 Output Pattern
 * @param dev - The device structure.
 * @param intf_chk_en - The Output Pattern mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_set_intf_chk_en(struct ad4080_dev *dev,
			   enum ad4080_intf_chk_en intf_chk_en)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4080_update_bits(dev, AD4080_REG_DATA_INTF_CONFIG_A,
				 AD4080_INTF_CHK_EN_MSK,
				 no_os_field_prep(AD4080_INTF_CHK_EN_MSK, intf_chk_en));
	if (ret)
		return ret;

	dev->intf_chk_en = intf_chk_en;

	return 0;
}

/**
 * @brief Get AD4080 Output Pattern
 * @param dev - The device structure.
 * @return Output Pattern value
 */
enum ad4080_intf_chk_en ad4080_get_intf_chk_en(struct ad4080_dev *dev)
{
	return dev->intf_chk_en;
}

/**
 * @brief Set AD4080 LVDS/SPI Lane Control
 * @param dev - The device structure.
 * @param cnv_spi_lvds_lanes - The LVDS/SPI Lane Control mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_set_cnv_spi_lvds_lanes(struct ad4080_dev *dev,
				  enum ad4080_cnv_spi_lvds_lanes cnv_spi_lvds_lanes)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4080_update_bits(dev, AD4080_REG_DATA_INTF_CONFIG_A,
				 AD4080_DATA_LANE_CNT_MSK,
				 no_os_field_prep(AD4080_DATA_LANE_CNT_MSK, cnv_spi_lvds_lanes));
	if (ret)
		return ret;

	dev->cnv_spi_lvds_lanes = cnv_spi_lvds_lanes;

	return 0;
}

/**
 * @brief Get AD4080 LVDS/SPI Lane Control
 * @param dev - The device structure.
 * @return LVDS/SPI Lane Control value
 */
enum ad4080_cnv_spi_lvds_lanes ad4080_get_cnv_spi_lvds_lanes(
	struct ad4080_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->cnv_spi_lvds_lanes;
}

/**
 * @brief Set AD4080 Data Interface Configuration
 * @param dev - The device structure.
 * @param conv_data_spi_lvds - The device mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_set_conv_data_spi_lvds(struct ad4080_dev *dev,
				  enum ad4080_conv_data_spi_lvds conv_data_spi_lvds)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4080_update_bits(dev, AD4080_REG_DATA_INTF_CONFIG_A,
				 AD4080_DATA_INTF_SEL_MSK,
				 no_os_field_prep(AD4080_DATA_INTF_SEL_MSK, conv_data_spi_lvds));
	if (ret)
		return ret;

	dev->conv_data_spi_lvds = conv_data_spi_lvds;

	return 0;
}

/**
 * @brief Get AD4080 Data Interface Configuration
 * @param dev - The device structure.
 * @return Data Interface Configuration value
 */
enum ad4080_conv_data_spi_lvds ad4080_get_conv_data_spi_lvds(
	struct ad4080_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->conv_data_spi_lvds;
}

/**
 * @brief Set AD4080 Interface clock periods from CNV rising edge
 * @param dev - The device structure.
 * @param lvds_cnv_clk_cnt - Interface clock periods from CNV rising edge value
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_set_lvds_cnv_clk_cnt(struct ad4080_dev *dev,
				enum ad4080_lvds_cnv_clk_cnt lvds_cnv_clk_cnt)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4080_update_bits(dev, AD4080_REG_DATA_INTF_CONFIG_B,
				 AD4080_LVDS_CNV_CLK_CNT_MSK,
				 no_os_field_prep( AD4080_LVDS_CNV_CLK_CNT_MSK, lvds_cnv_clk_cnt));
	if (ret)
		return ret;

	dev->lvds_cnv_clk_cnt = lvds_cnv_clk_cnt;

	return 0;
}

/**
 * @brief Get AD4080 Interface clock periods from CNV rising edge
 * @param dev - The device structure.
 * @return Interface clock periods from CNV rising edge value
 */
enum ad4080_lvds_cnv_clk_cnt ad4080_get_lvds_cnv_clk_cnt(struct ad4080_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->lvds_cnv_clk_cnt;
}

/**
 * @brief Set AD4080 LVDS Self Clock Mode
 * @param dev - The device structure.
 * @param lvds_self_clk_mode - The LVDS Self Clock Mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_set_lvds_self_clk_mode(struct ad4080_dev *dev,
				  enum ad4080_lvds_self_clk_mode lvds_self_clk_mode)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4080_update_bits(dev, AD4080_REG_DATA_INTF_CONFIG_B,
				 AD4080_LVDS_SELF_CLK_EN_MSK,
				 no_os_field_prep(AD4080_LVDS_SELF_CLK_EN_MSK, lvds_self_clk_mode));
	if (ret)
		return ret;

	dev->lvds_self_clk_mode = lvds_self_clk_mode;

	return 0;
}

/**
 * @brief Get AD4080 LVDS Self Clock Mode
 * @param dev - The device structure.
 * @return LVDS Self Clock Mode value
 */
enum ad4080_lvds_self_clk_mode ad4080_get_lvds_self_clk_mode(
	struct ad4080_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->lvds_self_clk_mode;
}

/**
 * @brief Set AD4080 LVDS CNV Clock Mode
 * @param dev - The device structure.
 * @param cnv_clk_mode - The LVDS CNV Clock Mode value
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_set_lvds_cnv_clk_mode(struct ad4080_dev *dev,
				 enum ad4080_lvds_cnv_clk_mode cnv_clk_mode)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4080_update_bits(dev, AD4080_REG_DATA_INTF_CONFIG_B,
				 AD4080_LVDS_CNV_EN,
				 no_os_field_prep(AD4080_LVDS_CNV_EN, cnv_clk_mode));
	if (ret)
		return ret;

	dev->cnv_clk_mode = cnv_clk_mode;

	return 0;
}

/**
 * @brief Get AD4080 LVDS CNV Clock Mode
 * @param dev - The device structure.
 * @return LVDS CNV Clock Mode value
 */
enum ad4080_lvds_cnv_clk_mode ad4080_get_lvds_cnv_clk_mode(
	struct ad4080_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->cnv_clk_mode;
}

/**
 * @brief Set AD4080 LVDS Differential Output Voltage
 * @param dev - The device structure.
 * @param lvds_vod - The LVDS Differential Output Voltage value
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_set_lvds_vod(struct ad4080_dev *dev,
			enum ad4080_lvds_vod lvds_vod)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4080_update_bits(dev, AD4080_REG_DATA_INTF_CONFIG_C,
				 AD4080_LVDS_VOD_MSK,
				 no_os_field_prep(AD4080_LVDS_VOD_MSK, lvds_vod));
	if (ret)
		return ret;

	dev->lvds_vod = lvds_vod;

	return 0;
}

/**
 * @brief Get AD4080 LVDS Differential Output Voltage
 * @param dev - The device structure.
 * @return LVDS Differential Output Voltage value
 */
enum ad4080_lvds_vod ad4080_get_lvds_vod(struct ad4080_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->lvds_vod;
}

/**
 * @brief Set AD4080 MSPI SDO Output Current
 * @param dev - The device structure.
 * @param mspi_drv - The MSPI SDO Output Current value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_set_mspi_drv(struct ad4080_dev *dev,
			enum ad4080_mspi_drv mspi_drv)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4080_update_bits(dev, AD4080_REG_DATA_INTF_CONFIG_C,
				 AD4080_MSPI_DRV_MSK,
				 no_os_field_prep(AD4080_MSPI_DRV_MSK, mspi_drv));
	if (ret)
		return ret;

	dev->mspi_drv = mspi_drv;

	return 0;
}

/**
 * @brief Get AD4080 MSPI SDO Output Current
 * @param dev - The device structure.
 * @return MSPI SDO Output Current value
 */
enum ad4080_mspi_drv ad4080_get_mspi_drv( struct ad4080_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->mspi_drv;
}

/**
 * @brief Set AD4080 Analog/Digital LDO
 * @param dev - The device structure.
 * @param ana_dig_ldo_pd - The Analog/Digital LDO value
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_set_ana_dig_ldo_pd(struct ad4080_dev *dev,
			      enum ad4080_ana_dig_ldo_pd ana_dig_ldo_pd)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4080_update_bits(dev, AD4080_REG_PWR_CTRL,
				 AD4080_ANA_DIG_LDO_PD_MSK,
				 no_os_field_prep(AD4080_ANA_DIG_LDO_PD_MSK, ana_dig_ldo_pd));
	if (ret)
		return ret;

	dev->ana_dig_ldo_pd = ana_dig_ldo_pd;

	return 0;
}

/**
 * @brief Get AD4080 Analog/Digital LDO
 * @param dev - The device structure.
 * @return MSPI Analog/Digital LDO value
 */
enum ad4080_ana_dig_ldo_pd ad4080_get_ana_dig_ldo_pd(struct ad4080_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->ana_dig_ldo_pd;
}

/**
 * @brief Set AD4080 Interface LDO
 * @param dev - The device structure.
 * @param intf_ldo_pd - The  Interface LDO value
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_set_intf_ldo_pd(struct ad4080_dev *dev,
			   enum ad4080_intf_ldo_pd intf_ldo_pd)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4080_update_bits(dev, AD4080_REG_PWR_CTRL,
				 AD4080_INTF_LDO_PD_MSK,
				 no_os_field_prep(AD4080_INTF_LDO_PD_MSK, intf_ldo_pd));
	if (ret)
		return ret;

	dev->intf_ldo_pd = intf_ldo_pd;

	return 0;
}

/**
 * @brief Get AD4080 Interface LDO
 * @param dev - The device structure.
 * @return Interface LDO value
 */
enum ad4080_intf_ldo_pd ad4080_get_intf_ldo_pd(struct ad4080_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->intf_ldo_pd;
}

/**
 * @brief Set AD4080 Conversion Data FIFO Mode
 * @param dev - The device structure.
 * @param fifo_mode - The Conversion Data FIFO Mode
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_set_fifo_mode(struct ad4080_dev *dev,
			 enum ad4080_fifo_mode fifo_mode)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad4080_update_bits(dev, AD4080_REG_FIFO_CONFIG,
				 AD4080_FIFO_MODE_MSK,
				 no_os_field_prep(AD4080_FIFO_MODE_MSK, fifo_mode));
	if (ret)
		return ret;

	dev->fifo_mode = fifo_mode;

	return 0;
}

/**
 * @brief Get Set AD4080 Conversion Data FIFO Mode
 * @param dev - The device structure.
 * @return AD4080 Conversion Data FIFO Mode value
 */
enum ad4080_fifo_mode ad4080_get_fifo_mode(struct ad4080_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return dev->fifo_mode;
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_init(struct ad4080_dev **device,
		struct ad4080_init_param init_param)
{
	struct ad4080_dev *dev;
	uint8_t data;
	int ret;

	dev = (struct ad4080_dev *)calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* SPI Initialization*/
	ret = no_os_spi_init(&dev->spi_desc, init_param.spi_init);
	if (ret)
		goto error_dev;

	ret = ad4080_read(dev, AD4080_REG_CHIP_TYPE, &data);
	if (ret)
		goto error_spi;

	if (data != AD4080_CHIP_ID)
		goto error_spi;

	/* Software Reset */
	ret = ad4080_soft_reset(dev);
	if (ret)
		goto error_spi;

	dev->spi3wire = init_param.spi3wire;
	dev->addr_asc = init_param.addr_asc;

	ret = ad4080_update_bits(dev, AD4080_REG_INTERFACE_CONFIG_A,
				 AD4080_ADDR_ASC_MSK |
				 AD4080_SDO_ENABLE_MSK,
				 no_os_field_prep(AD4080_SDO_ENABLE_MSK, dev->spi3wire) |
				 no_os_field_prep(AD4080_ADDR_ASC_MSK, dev->addr_asc));
	if (ret)
		goto error_spi;

	*device = dev;

	return 0;

error_spi:
	no_os_spi_remove(dev->spi_desc);
error_dev:
	free(dev);

	return ret;
}

/**
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad4080_remove(struct ad4080_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	free(dev);

	return ret;
}
