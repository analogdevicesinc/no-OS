/***************************************************************************//**
 *   @file   max14001.c
 *   @brief  Implementation of max14001.c
 *   @author NAlteza (nathaniel.alteza@analog.com)
 *******************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
 ******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include "max14001.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, fail otherwise.
 */
int max14001_read(struct max14001_dev *dev,
		  uint8_t reg_addr,
		  uint16_t *reg_data)
{
	int ret;
	uint8_t buf[2];
	uint16_t tx_data;

	if (((reg_addr > MAX14001_WEN_REG) & (reg_addr < MAX14001_FLTV_REG)) \
	    || reg_addr > MAX14001_ENBLV_REG \
	    || reg_addr == MAX14001_ACT_REG)
		return -EINVAL;

	tx_data = MAX14001_REG_READ(reg_addr);
	tx_data = REVERSE_UINT16(tx_data);

	no_os_put_unaligned_be16(tx_data, buf);

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 2);
	if (ret)
		return ret;

	no_os_put_unaligned_be16(tx_data, buf);

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 2);
	if (ret)
		return ret;

	*reg_data = no_os_get_unaligned_be16(buf);
	*reg_data = REVERSE_UINT16(*reg_data);
	*reg_data &= MAX14001_REG_DATA_MASK;

	return 0;
}

/**
 * @brief Write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, fail otherwise.
 */
int max14001_write(struct max14001_dev *dev,
		   uint8_t reg_addr,
		   uint16_t reg_data)
{
	uint8_t buf[2];
	uint16_t tx_data;

	if (((reg_addr > MAX14001_WEN_REG) & (reg_addr < MAX14001_FLTV_REG)) \
	    || reg_addr > MAX14001_ENBLV_REG \
	    || reg_addr < MAX14001_FLTEN_REG)
		return -EINVAL;

	tx_data = MAX14001_REG_WRITE(reg_addr,reg_data);
	tx_data = REVERSE_UINT16(tx_data);

	no_os_put_unaligned_be16(tx_data, buf);

	return no_os_spi_write_and_read(dev->spi_desc, buf, 2);
}

/**
 * @brief SPI write to device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, fail otherwise.
 */
int max14001_reg_update(struct max14001_dev *dev,
			uint8_t reg_addr,
			uint16_t mask,
			uint16_t data)
{
	uint16_t reg_data;
	int16_t ret;

	if (((reg_addr > MAX14001_WEN_REG) & (reg_addr < MAX14001_FLTV_REG)) \
	    || reg_addr > MAX14001_ENBLV_REG \
	    || reg_addr < MAX14001_FLTEN_REG)
		return -EINVAL;

	ret = max14001_read(dev, reg_addr, &reg_data);
	if (ret)
		return ret;

	reg_data &= ~mask;
	reg_data |= data;

	return max14001_write(dev, reg_addr, reg_data);
}

/**
 * @brief Write to config registers then to corresponding verification register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, fail otherwise.
 */
int max14001_write_config_verify(struct max14001_dev *dev,
				 uint8_t reg_addr,
				 uint16_t mask,
				 uint16_t data)
{
	uint16_t reg_data;
	int16_t ret;

	if (((reg_addr > MAX14001_WEN_REG) & (reg_addr < MAX14001_FLTV_REG)) \
	    || reg_addr > MAX14001_ENBLV_REG \
	    || reg_addr < MAX14001_FLTEN_REG)
		return -EINVAL;

	ret = max14001_read(dev, reg_addr, &reg_data);
	if (ret)
		return ret;

	reg_data &= ~mask;
	reg_data |= data;

	ret = max14001_write(dev, reg_addr, reg_data);
	if (ret)
		return ret;

	return max14001_write(dev, VERIFICATION_REG(reg_addr), reg_data);
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 *                     parameters.
 * @return 0 in case of success, fail otherwise.
 */
int max14001_init(struct max14001_dev **device,
		  struct max14001_init_param init_param)
{
	struct max14001_dev *dev;
	int ret;

	no_os_mdelay(100);

	dev = (struct max14001_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
	if (ret)
		goto error_dev;

	*device = dev;

	return ret;

error_dev:
	no_os_free(dev);

	return ret;

}

/**
 * @brief Initialize the configurations of device.
 * @param dev - The device structure.
 * @return 0 in case of success, fail otherwise.
 */
int max14001_init_config(struct max14001_dev *dev)
{
	int i, ret;
	uint16_t data;

	no_os_mdelay(100);

	ret = max14001_read(dev, MAX14001_FLAGS_REG, &data);
	if (ret)
		return ret;

	ret = max14001_wen(dev, true);
	if (ret)
		return ret;

	ret = max14001_reg_update(dev, MAX14001_FLTEN_REG,
				  MAX14001_MV_FLAG_MASK, 0);
	if (ret)
		return ret;

	for (i = MAX14001_FLTEN_REG; i <= MAX14001_ENBL_REG; i++) {
		ret = max14001_write(dev, i, 0x000);
		if (ret)
			return ret;
	}

	for (i = MAX14001_FLTV_REG; i <= MAX14001_ENBLV_REG; i++) {
		ret = max14001_write(dev, i, 0x000);
		if (ret)
			return ret;
	}

	ret = max14001_emv_config(dev, true);
	if (ret)
		return ret;

	ret = max14001_wen(dev, false);
	if (ret)
		return ret;

	for (i = MAX14001_FLTEN_REG; i <= MAX14001_ENBL_REG; i++) {
		ret = max14001_read(dev, i, &data);
		if (ret)
			return ret;
	}

	ret = max14001_read(dev, MAX14001_FLAGS_REG, &data);
	if (ret)
		return ret;

	ret = max14001_read(dev, MAX14001_FLAGS_REG, &data);
	if (ret)
		return ret;

	ret = max14001_wen(dev, true);
	if (ret)
		return ret;

	ret = max14001_ena_config(dev, true);
	if (ret)
		return ret;

	ret = max14001_read(dev, MAX14001_FLAGS_REG, &data);
	if (ret)
		return ret;

	ret = max14001_read(dev, MAX14001_FLAGS_REG, &data);
	if (ret)
		return ret;

	return max14001_wen(dev, false);
}

/**
 * @brief Enable write to registers.
 * @param dev - The device structure.
 * @param enable - 	true to write enable
 *               	false to write disable
 * @return 0 in case of success, fail otherwise.
 */
int max14001_wen(struct max14001_dev *dev, bool enable)
{
	return max14001_write(dev, MAX14001_WEN_REG,
			      enable ? MAX14001_SPI_REG_WRITE_ENABLE:
			      MAX14001_SPI_REG_WRITE_DISABLE);
}

/**
 * @brief Full reset, same effect as power on reset.
 * @param dev - The device structure.
 * @return 0 in case of success, fail otherwise.
 */
int max14001_full_reset(struct max14001_dev *dev)
{
	return max14001_write(dev, MAX14001_ACT_REG, MAX14001_RSET_MASK);
}

/**
 * @brief Software reset. Restores all registers to their POR value.
 * @param dev - The device structure.
 * @return 0 in case of success, fail otherwise.
 */
int max14001_reg_reset(struct max14001_dev *dev)
{
	return max14001_write(dev, MAX14001_ACT_REG, MAX14001_SRES_MASK);
}

/**
 * @brief Trigger an inrush current pulse.
 * @param dev - The device structure.
 * @return 0 in case of success, fail otherwise.
 */
int max14001_inpls_reset(struct max14001_dev *dev)
{
	return max14001_write(dev, MAX14001_ACT_REG, MAX14001_INPLS_MASK);
}

/**
 * @brief Enable MV fault.
 * @param dev - The device structure.
 * @param mode - 	true to allow MV error to assert FAULT
 *               	false to prevent MV error to assert FAULT (default)
 * @return 0 in case of success, fail otherwise.
*/
int max14001_emv_config(struct max14001_dev *dev,
			bool mode)
{
	return max14001_write_config_verify(dev, MAX14001_FLTEN_REG,
					    MAX14001_EMV_FLTEN_MASK,
					    mode ? MAX14001_EMV_FLTEN_MASK: 0);
}

/**
 * @brief Enable FET fault.
 * @param dev - The device structure.
 * @param mode - 	true to allow FET error to assert FAULT
 *               	false to prevent FET error to assert FAULT (default)
 * @return 0 in case of success, fail otherwise.
 */
int max14001_efet_config(struct max14001_dev *dev,
			 bool mode)
{
	return max14001_write_config_verify(dev, MAX14001_FLTEN_REG,
					    MAX14001_EFET_FLTEN_MASK,
					    mode ? MAX14001_EFET_FLTEN_MASK: 0);
}

/**
 * @brief Enable CRCF fault.
 * @param dev - The device structure.
 * @param mode - 	true to allow CRCF error to assert FAULT
 *               	false to prevent CRCF error to assert FAULT (default)
 * @return 0 in case of success, fail otherwise.
 */
int max14001_ecrcf_config(struct max14001_dev *dev,
			  bool mode)
{
	return max14001_write_config_verify(dev, MAX14001_FLTEN_REG,
					    MAX14001_ECRCF_FLTEN_MASK,
					    mode ? MAX14001_ECRCF_FLTEN_MASK: 0);
}

/**
 * @brief Enable CRCL fault.
 * @param dev - The device structure.
 * @param mode - 	true to allow CRCL error to assert FAULT
 *               	false to prevent CRCL error to assert FAULT (default)
 * @return 0 in case of success, fail otherwise.
 */
int max14001_ecrcl_config(struct max14001_dev *dev,
			  bool mode)
{
	return max14001_write_config_verify(dev, MAX14001_FLTEN_REG,
					    MAX14001_ECRCL_FLTEN_MASK,
					    mode ? MAX14001_ECRCL_FLTEN_MASK: 0);
}

/**
 * @brief Enable COM fault.
 * @param dev - The device structure.
 * @param mode - 	true to allow COM error to assert FAULT
 *               	false to prevent COM error to assert FAULT (default)
 * @return 0 in case of success, fail otherwise.
 */
int max14001_ecom_config(struct max14001_dev *dev,
			 bool mode)
{
	return max14001_write_config_verify(dev, MAX14001_FLTEN_REG,
					    MAX14001_ECOM_FLTEN_MASK,
					    mode ? MAX14001_ECOM_FLTEN_MASK: 0);
}

/**
 * @brief Enable SPI fault.
 * @param dev - The device structure.
 * @param mode - 	true to allow SPI error to assert FAULT
 *               	false to prevent SPI error to assert FAULT (default)
 * @return 0 in case of success, fail otherwise.
 */
int max14001_espi_config(struct max14001_dev *dev,
			 bool mode)
{
	return max14001_write_config_verify(dev, MAX14001_FLTEN_REG,
					    MAX14001_ESPI_FLTEN_MASK,
					    mode ? MAX14001_ESPI_FLTEN_MASK: 0);
}

/**
 * @brief Enable INRD fault.
 * @param dev - The device structure.
 * @param mode - 	true to allow INRD error to assert FAULT
 *               	false to prevent INRD error to assert FAULT (default)
 * @return 0 in case of success, fail otherwise.
 */
int max14001_einrd_config(struct max14001_dev *dev,
			  bool mode)
{
	return max14001_write_config_verify(dev, MAX14001_FLTEN_REG,
					    MAX14001_EINRD_FLTEN_MASK,
					    mode ? MAX14001_EINRD_FLTEN_MASK: 0);
}

/**
 * @brief Enable ADC fault.
 * @param dev - The device structure.
 * @param mode - 	true to allow ADC error to assert FAULT
 *               	false to prevent ADC error to assert FAULT (default)
 * @return 0 in case of success, fail otherwise.
 */
int max14001_eadc_config(struct max14001_dev *dev,
			 bool mode)
{
	return max14001_write_config_verify(dev, MAX14001_FLTEN_REG,
					    MAX14001_EADC_FLTEN_MASK,
					    mode ? MAX14001_EADC_FLTEN_MASK: 0);
}

/**
 * @brief Enable dynamic FAULT signal.
 * @param dev - The device structure.
 * @param mode - 	true for dynamic FAULT signal
 *               	false for latched FAULT signal (default)
 * @return 0 in case of success, fail otherwise.
 */
int max14001_dyen_config(struct max14001_dev *dev,
			 bool mode)
{
	return max14001_write_config_verify(dev, MAX14001_FLTEN_REG,
					    MAX14001_DYEN_FLTEN_MASK,
					    mode ? MAX14001_DYEN_FLTEN_MASK: 0);
}

/**
 * @brief Enable/Disable fast inrush mode.
 * @param dev - The device structure.
 * @param fast - 	true for FAST inrush mode (default)
 *               	false for ADC controlled
 * @return 0 in case of success, fail otherwise.
 */
int max14001_fast_config(struct max14001_dev *dev,
			 bool fast)
{
	return max14001_write_config_verify(dev, MAX14001_CFG_REG,
					    MAX14001_FAST_CFG_MASK,
					    fast ? MAX14001_FAST_CFG_MASK: 0);
}

/**
 * @brief Selects Inrush comparator input multiplexer.
 * @param dev - The device structure.
 * @param raw_data - 	0 to connect comparator to ADC register(default)
 *               	1 to connect comparator to FADC register
 * @return 0 in case of success, fail otherwise.
 */
int max14001_iraw_config(struct max14001_dev *dev,
			 bool raw_data)
{
	return max14001_write_config_verify(dev, MAX14001_CFG_REG,
					    MAX14001_IRAW_CFG_MASK,
					    raw_data ? MAX14001_IRAW_CFG_MASK: 0);
}

/**
 * @brief Enable/Disable field-side current sink.
 * @param dev - The device structure.
 * @param enable - 	true to enable field-side current sink
 *               	false to prevent field-side current sink (default)
 * @return 0 in case of success, fail otherwise.
 */
int max14001_ena_config(struct max14001_dev *dev,
			bool enable)
{
	return max14001_write_config_verify(dev, MAX14001_ENBL_REG,
					    MAX14001_ENA_ENBL_MASK,
					    enable ? MAX14001_ENA_ENBL_MASK: 0);
}

/**
 * @brief Selects the voltage reference source for the ADC.
 * @param dev - The device structure.
 * @param mode - 	0 for external voltage reference enabled
 *               	1 for internal voltage reference enabled (default)
 * @return 0 in case of success, fail otherwise.
 */
int max14001_exrf_config(struct max14001_dev *dev,
			 bool mode)
{
	return max14001_write_config_verify(dev, MAX14001_CFG_REG,
					    MAX14001_EXRF_CFG_MASK,
					    mode ? MAX14001_EXRF_CFG_MASK: 0);
}

/**
 * @brief Current source connection.
 * @param dev - The device structure.
 * @param mode - 	0 for current source on and connected to the REFIN pin
 *               	1 for current source off (default)
 * @return 0 in case of success, fail otherwise.
 */
int max14001_exti_config(struct max14001_dev *dev,
			 bool mode)
{
	return max14001_write_config_verify(dev, MAX14001_CFG_REG,
					    MAX14001_EXTI_CFG_MASK,
					    mode ? MAX14001_EXTI_CFG_MASK: 0);
}

/**
 * @brief Configure 4-bit inrush time.
 * @param dev - The device structure.
 * @param mode - 	0 to 120ms in 8ms increments.
 * 			Inputs are clamped and rounded within limits.
 * @return 0 in case of success, fail otherwise.
 */
int max14001_tinr_config(struct max14001_dev *dev,
			 int mode)
{
	return max14001_write_config_verify(dev, MAX14001_INRP_REG,
					    MAX14001_TINR_INRP_MASK,
					    MAX14001_TINR_INRP_MODE(mode));
}

/**
 * @brief Configure 4-bit inrush current.
 * @param dev - The device structure.
 * @param mode - 	50uA to 105mA in 7mA increments.
 * 			Inputs are clamped and rounded within limits.
 * @return 0 in case of success, fail otherwise.
 */
int max14001_iinr_config(struct max14001_dev *dev,
			 int mode)
{
	return max14001_write_config_verify(dev, MAX14001_INRP_REG,
					    MAX14001_IINR_INRP_MASK,
					    MAX14001_IINR_INRP_MODE(mode));
}

/**
 * @brief Set maximum duty cycle for inrush current over the last 10 seconds.
 * @param dev - The device structure.
 * @param mode - Duty Cycle
 * 		Accepted values: DUTY_OFF
 * 			DUTY_1P6
 * 			DUTY_3P1
 * 			DUTY_6P3
 * @return 0 in case of success, fail otherwise.
 */
int max14001_du_config(struct max14001_dev *dev,
		       enum max14001_du mode)
{
	if (mode < DUTY_OFF || mode > DUTY_6P3)
		return -EINVAL;
	return max14001_write_config_verify(dev, MAX14001_INRP_REG,
					    MAX14001_DU_INRP_MASK,
					    MAX14001_DU_INRP_MODE(mode));
}

/**
 * @brief Configure 4-bit bias current.
 * @param dev - The device structure.
 * @param mode - 	50uA to 3.75mA in 0.25mA increments.
 * 			Inputs are clamped and rounded within limits.
 * @return 0 in case of success, fail otherwise.
 */
int max14001_ibias_config(struct max14001_dev *dev,
			  float mode)
{
	return max14001_write_config_verify(dev, MAX14001_CFG_REG,
					    MAX14001_IBIAS_CFG_MASK,
					    MAX14001_IBIAS_CFG_MODE(mode));
}

/**
 * @brief Set number of readings averaged in the ADC filter.
 * @param dev - The device structure.
 * @param mode - Average readings
 * 		Accepted values: FILTER_OFF
 * 			AVERAGE_2_READINGS
 * 			AVERAGE_4_READINGS
 * 			AVERAGE_8_READINGS
 * @return 0 in case of success, fail otherwise.
 */
int max14001_ft_config(struct max14001_dev *dev,
		       enum max14001_ft mode)
{
	if (mode < FILTER_OFF || mode > AVERAGE_8_READINGS)
		return -EINVAL;

	return max14001_write_config_verify(dev, MAX14001_CFG_REG,
					    MAX14001_FT_CFG_MASK,
					    MAX14001_FT_CFG_MODE(mode));
}

/**
 * @brief Get adc data.
 * @param dev - Device handler.
 * @param data - Pointer to raw adc value.
 * @return 0 in case of success, negative error code otherwise.
 */
int max14001_get_data_raw(struct max14001_dev *dev, uint16_t *data)
{
	if (!dev)
		return -EINVAL;

	return max14001_read(dev, MAX14001_ADC_REG, data);
}

/**
 * @brief Get filtered adc data.
 * @param dev - Device handler.
 * @param data - Pointer to filtered adc value.
 * @return 0 in case of success, negative error code otherwise.
 */
int max14001_get_data_filtered(struct max14001_dev *dev, uint16_t *data)
{
	if (!dev)
		return -EINVAL;

	return max14001_read(dev, MAX14001_FADC_REG, data);
}

/**
 * @brief Free the resources allocated by max14001_init().
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int max14001_remove(struct max14001_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}
