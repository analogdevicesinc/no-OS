/***************************************************************************//**
*   @file    ad7124.c
*   @brief   AD7124 implementation file.
*   	     Devices: AD7124-4, AD7124-8
*
********************************************************************************
* Copyright 2015-2019, 2023(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <stdbool.h>
#include "ad7124.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

/*
 * Post reset delay required to ensure all internal config done
 * A time of 2ms should be enough based on the data sheet, but 4ms
 * chosen to provide enough margin, in case mdelay is not accurate.
 */
#define AD7124_POST_RESET_DELAY	4

/***************************************************************************//**
 * @brief Reads the value of the specified register without checking if the
 *        device is ready to accept user requests.
 * @param dev   - The handler of the instance of the driver.
 * @param p_reg - Pointer to the register structure holding info about the
 *               register to be read. The read value is stored inside the
 *               register structure.
 * @return Returns 0 for success or negative error code otherwise.
*******************************************************************************/
int32_t ad7124_no_check_read_register(struct ad7124_dev *dev,
				      struct ad7124_st_reg* p_reg)
{
	int32_t ret = 0;
	uint8_t buffer[8] = { 0 };
	uint8_t i = 0;
	uint8_t check8 = 0, add_status_length = 0;
	uint8_t msg_buf[8] = { 0 };

	if (!dev || !p_reg)
		return -EINVAL;

	/* Build the Command word */
	buffer[0] = AD7124_COMM_REG_WEN | AD7124_COMM_REG_RD |
		    AD7124_COMM_REG_RA(p_reg->addr);

	/*
	 * If this is an AD7124_DATA register read, and the DATA_STATUS bit is set
	 * in ADC_CONTROL, need to read 4, not 3 bytes for DATA with STATUS
	 */
	if ((p_reg->addr == AD7124_DATA_REG) &&
	    (dev->regs[AD7124_ADC_Control].value & AD7124_ADC_CTRL_REG_DATA_STATUS))
		add_status_length = 1;

	/* Read data from the device */
	ret = no_os_spi_write_and_read(dev->spi_desc,
				       buffer,
				       ((dev->use_crc != AD7124_DISABLE_CRC) ? p_reg->size + 1
					: p_reg->size) + 1 + add_status_length);
	if (ret)
		return ret;

	/* Check the CRC */
	if (dev->use_crc == AD7124_USE_CRC) {
		msg_buf[0] = AD7124_COMM_REG_WEN | AD7124_COMM_REG_RD |
			     AD7124_COMM_REG_RA(p_reg->addr);
		for (i = 1; i < p_reg->size + 2 + add_status_length; ++i)
			msg_buf[i] = buffer[i];
		check8 = ad7124_compute_crc8(msg_buf, p_reg->size + 2 + add_status_length);
	}

	if (check8)
		/* ReadRegister checksum failed. */
		return -EBADMSG;

	/*
	 * if reading Data with 4 bytes, need to copy the status byte to the STATUS
	 * register struct value member
	 */
	if (add_status_length)
		dev->regs[AD7124_Status].value = buffer[p_reg->size + 1];

	/* Build the result */
	p_reg->value = 0;
	for (i = 1; i < p_reg->size + 1; i++) {
		p_reg->value <<= 8;
		p_reg->value += buffer[i];
	}

	return 0;
}

/***************************************************************************//**
 * @brief Writes the value of the specified register without checking if the
 *        device is ready to accept user requests.
 * @param dev - The handler of the instance of the driver.
 * @param reg - Register structure holding info about the register to be written
 * @return Returns 0 for success or negative error code otherwise.
*******************************************************************************/
int32_t ad7124_no_check_write_register(struct ad7124_dev *dev,
				       struct ad7124_st_reg reg)
{
	int32_t reg_value = 0;
	uint8_t wr_buf[8] = { 0 };
	uint8_t i = 0;
	uint8_t crc8 = 0;

	if (!dev)
		return -EINVAL;

	/* Build the Command word */
	wr_buf[0] = AD7124_COMM_REG_WEN | AD7124_COMM_REG_WR |
		    AD7124_COMM_REG_RA(reg.addr);

	/* Fill the write buffer */
	reg_value = reg.value;
	for (i = 0; i < reg.size; i++) {
		wr_buf[reg.size - i] = reg_value & 0xFF;
		reg_value >>= 8;
	}

	/* Compute the CRC */
	if (dev->use_crc != AD7124_DISABLE_CRC) {
		crc8 = ad7124_compute_crc8(wr_buf, reg.size + 1);
		wr_buf[reg.size + 1] = crc8;
	}

	return no_os_spi_write_and_read(dev->spi_desc,
					wr_buf,
					(dev->use_crc != AD7124_DISABLE_CRC) ? reg.size + 2
					: reg.size + 1);
}

/***************************************************************************//**
 * @brief Reads the value of the specified register only when the device is ready
 *        to accept user requests. If the device ready flag is deactivated the
 *        read operation will be executed without checking the device state.
 *        DEPRECATED, use ad7124_read_register2.
 * @param dev   - The handler of the instance of the driver.
 * @param p_reg - Pointer to the register structure holding info about the
 *               register to be read. The read value is stored inside the
 *               register structure.
 * @return Returns 0 for success or negative error code otherwise.
*******************************************************************************/
int32_t ad7124_read_register(struct ad7124_dev *dev,
			     struct ad7124_st_reg* p_reg)
{
	int32_t ret;

	if (p_reg->addr != AD7124_ERR_REG && dev->check_ready) {
		ret = ad7124_wait_for_spi_ready(dev,
						dev->spi_rdy_poll_cnt);
		if (ret)
			return ret;
	}

	return ad7124_no_check_read_register(dev,
					     p_reg);
}

/***************************************************************************//**
 * @brief Wrap the read register function to give it a modern signature.
 * @param [in] dev - Driver handler pointer.
 * @param [in] reg - Address of the register to be read.
 * @param [out] readval - Pointer to the register value.
 * @return Returns 0 for success or negative error code otherwise.
***************************************************************************/
int32_t ad7124_read_register2(struct ad7124_dev *dev,
			      uint32_t reg,
			      uint32_t *readval)
{
	int32_t ret;

	ret = ad7124_read_register(dev, &dev->regs[reg]);
	if (ret)
		return ret;

	*readval = dev->regs[reg].value;

	return 0;
}

/***************************************************************************//**
 * @brief Writes the value of the specified register only when the device is
 *        ready to accept user requests. If the device ready flag is deactivated
 *        the write operation will be executed without checking the device state.
 *        DEPRECATED, use ad7124_write_register2.
 * @param dev - The handler of the instance of the driver.
 * @param p_reg - Register structure holding info about the register to be written
 * @return Returns 0 for success or negative error code otherwise.
********************************************************************************/
int32_t ad7124_write_register(struct ad7124_dev *dev,
			      struct ad7124_st_reg p_reg)
{
	int32_t ret;

	if (dev->check_ready) {
		ret = ad7124_wait_for_spi_ready(dev,
						dev->spi_rdy_poll_cnt);
		if (ret)
			return ret;
	}

	return ad7124_no_check_write_register(dev,
					      p_reg);
}

/***************************************************************************//**
 * @brief Wrap the write register function to give it a modern signature.
 * @param [in] dev - Driver handler pointer.
 * @param [in] reg - Address of the register to be read.
 * @param [in] writeval - New value for the register.
 * @return 0 in case of success, error code otherwise.
******************************************************************************/
int32_t ad7124_write_register2(struct ad7124_dev *dev,
			       uint32_t reg,
			       uint32_t writeval)
{
	dev->regs[reg].value = writeval;

	return ad7124_write_register(dev, dev->regs[reg]);
}

/***************************************************************************//**
 * @brief Resets the device.
 * @param dev - The handler of the instance of the driver.
 * @return Returns 0 for success or negative error code otherwise.
*******************************************************************************/
int32_t ad7124_reset(struct ad7124_dev *dev)
{
	int32_t ret = 0;
	uint8_t wr_buf[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

	if (!dev)
		return -EINVAL;

	ret = no_os_spi_write_and_read(dev->spi_desc, wr_buf, 8);
	if (ret)
		return ret;

	/* CRC is disabled after reset */
	dev->use_crc = AD7124_DISABLE_CRC;

	/* Read POR bit to clear */
	ret = ad7124_wait_to_power_on(dev, dev->spi_rdy_poll_cnt);
	if (ret)
		return ret;

	no_os_mdelay(AD7124_POST_RESET_DELAY);

	return 0;
}

/***************************************************************************//**
 * @brief Waits until the device can accept read and write user actions.
 * @param dev     - The handler of the instance of the driver.
 * @param timeout - Count representing the number of polls to be done until the
 *                  function returns.
 * @return Returns 0 for success or negative error code otherwise.
*******************************************************************************/
int32_t ad7124_wait_for_spi_ready(struct ad7124_dev *dev,
				  uint32_t timeout)
{
	struct ad7124_st_reg *regs;
	int32_t ret;
	int8_t ready = 0;

	if (!dev)
		return -EINVAL;

	regs = dev->regs;

	while (!ready && --timeout) {
		/* Read the value of the Error Register */
		ret = ad7124_read_register(dev, &regs[AD7124_Error]);
		if (ret)
			return ret;

		/* Check the SPI IGNORE Error bit in the Error Register */
		ready = (regs[AD7124_Error].value &
			 AD7124_ERR_REG_SPI_IGNORE_ERR) == 0;
	}

	if (!timeout)
		return -ETIMEDOUT;

	return 0;
}

/***************************************************************************//**
 * @brief Waits until the device finishes the power-on reset operation.
 * @param dev     - The handler of the instance of the driver.
 * @param timeout - Count representing the number of polls to be done until the
 *                  function returns.
 * @return Returns 0 for success or negative error code otherwise.
*******************************************************************************/
int32_t ad7124_wait_to_power_on(struct ad7124_dev *dev,
				uint32_t timeout)
{
	struct ad7124_st_reg *regs;
	int32_t ret;
	int8_t powered_on = 0;

	if (!dev)
		return -EINVAL;

	regs = dev->regs;

	while (!powered_on && timeout--) {
		ret = ad7124_read_register(dev,
					   &regs[AD7124_Status]);
		if (ret)
			return ret;

		/* Check the POR_FLAG bit in the Status Register */
		powered_on = (regs[AD7124_Status].value &
			      AD7124_STATUS_REG_POR_FLAG) == 0;
	}

	if (!(timeout || powered_on))
		return -ETIMEDOUT;

	return 0;
}

/***************************************************************************//**
 * @brief Waits until a new conversion result is available.
 * @param dev     - The handler of the instance of the driver.
 * @param timeout - Count representing the number of polls to be done until the
 *                  function returns if no new data is available.
 * @return Returns 0 for success or negative error code otherwise.
*******************************************************************************/
int32_t ad7124_wait_for_conv_ready(struct ad7124_dev *dev,
				   uint32_t timeout)
{
	struct ad7124_st_reg *regs;
	int32_t ret;
	int8_t ready = 0;

	if (!dev)
		return -EINVAL;

	regs = dev->regs;

	while (!ready && --timeout) {
		/* Read the value of the Status Register */
		ret = ad7124_read_register(dev, &regs[AD7124_Status]);
		if (ret)
			return ret;
		/* Check the RDY bit in the Status Register */
		ready = (regs[AD7124_Status].value &
			 AD7124_STATUS_REG_RDY) == 0;
	}

	if (!timeout)
		return -ETIMEDOUT;

	return 0;
}

/***************************************************************************//**
 * @brief Reads the conversion result from the device.
 * @param dev     - The handler of the instance of the driver.
 * @param p_data  - Pointer to store the read data.
 * @return Returns 0 for success or negative error code otherwise.
*******************************************************************************/
int32_t ad7124_read_data(struct ad7124_dev *dev,
			 int32_t* p_data)
{
	struct ad7124_st_reg *regs;
	int32_t ret;

	if (!dev)
		return -EINVAL;

	regs = dev->regs;

	/* Read the value of the Status Register */
	ret = ad7124_read_register(dev, &regs[AD7124_Data]);

	/* Get the read result */
	*p_data = regs[AD7124_Data].value;

	return 0;
}

/***************************************************************************//**
 * @brief Get the ID of the channel of the latest conversion.
 * @param dev     - The handler of the instance of the driver.
 * @param status  - Pointer to store the read data.
 * @return Returns 0 for success or negative error code otherwise.
*******************************************************************************/
int32_t ad7124_get_read_chan_id(struct ad7124_dev *dev, uint32_t *status)
{
	int32_t ret;
	uint32_t reg_temp;

	ret = ad7124_read_register2(dev, AD7124_STATUS_REG, &reg_temp);
	if (ret)
		return ret;

	*status = reg_temp & AD7124_STATUS_REG_CH_ACTIVE(0xF);

	return 0;
}

/***************************************************************************//**
 * @brief Computes the CRC checksum for a data buffer.
 * @param p_buf    - Data buffer
 * @param buf_size - Data buffer size in bytes
 * @return Returns the computed CRC checksum.
*******************************************************************************/
uint8_t ad7124_compute_crc8(uint8_t * p_buf, uint8_t buf_size)
{
	uint8_t i = 0;
	uint8_t crc = 0;

	while (buf_size) {
		for (i = 0x80; i != 0; i >>= 1) {
			bool cmp1 = (crc & 0x80) != 0;
			bool cmp2 = (*p_buf & i) != 0;
			if (cmp1 != cmp2) {
				/* MSB of CRC register XOR input Bit from Data */
				crc <<= 1;
				crc ^= AD7124_CRC8_POLYNOMIAL_REPRESENTATION;
			} else {
				crc <<= 1;
			}
		}
		p_buf++;
		buf_size--;
	}

	return crc;
}

/***************************************************************************//**
 * @brief Updates the CRC settings.
 * @param dev - The handler of the instance of the driver.
 * @return None.
*******************************************************************************/
void ad7124_update_crcsetting(struct ad7124_dev *dev)
{
	struct ad7124_st_reg *regs;

	if (!dev)
		return;

	regs = dev->regs;

	/* Get CRC State. */
	if (regs[AD7124_Error_En].value & AD7124_ERREN_REG_SPI_CRC_ERR_EN)
		dev->use_crc = AD7124_USE_CRC;
	else
		dev->use_crc = AD7124_DISABLE_CRC;
}

/***************************************************************************//**
 * @brief Updates the device SPI interface settings.
 * @param dev - The handler of the instance of the driver.
 * @return None.
*******************************************************************************/
void ad7124_update_dev_spi_settings(struct ad7124_dev *dev)
{
	struct ad7124_st_reg *regs;

	if (!dev)
		return;

	regs = dev->regs;

	if (regs[AD7124_Error_En].value & AD7124_ERREN_REG_SPI_IGNORE_ERR_EN)
		dev->check_ready = 1;
	else
		dev->check_ready = 0;
}

/***************************************************************************//**
 * @brief Get the AD7124 reference clock.
 * @param [in] dev - Pointer to the application handler.
 * @param [out] f_clk - Pointer to the clock frequency container.
 * @return Returns 0 for success or negative error code otherwise.
******************************************************************************/
int32_t ad7124_fclk_get(struct ad7124_dev *dev, float *f_clk)
{
	int32_t ret;
	const float	f_clk_fp = 614400,
			f_clk_mp = 153600,
			f_clk_lp = 76800;
	uint32_t reg_temp;

	ret = ad7124_read_register2(dev, AD7124_ADC_Control, &reg_temp);
	if (ret)
		return ret;

	switch (dev->power_mode) {
	case 0:
		*f_clk = f_clk_lp;
		break;
	case 1:
		*f_clk = f_clk_mp;
		break;
	case 2:
	case 3:
		*f_clk = f_clk_fp;
		break;
	default:
		return ret;
	}

	return 0;
}

/***************************************************************************//**
 * @brief Get the filter coefficient for the sample rate.
 * @param [in] dev - Pointer to the application handler.
 * @param [in] chn_num - Channel number.
 * @param [out] flt_coff - Pointer to the filter coefficient container.
 * @return Returns 0 for success or negative error code otherwise.
******************************************************************************/
int32_t ad7124_fltcoff_get(struct ad7124_dev *dev,
			   int16_t chn_num,
			   uint16_t *flt_coff)
{
	uint16_t power_mode;
	int32_t ret;
	uint32_t reg_temp;

	ret = ad7124_read_register2(dev, AD7124_ADC_Control, &reg_temp);
	if (ret)
		return ret;

	power_mode = dev->power_mode;

	ret = ad7124_read_register2(dev, (AD7124_Filter_0 + chn_num), &reg_temp);
	if (ret)
		return ret;

	*flt_coff = 32;
	if (reg_temp & AD7124_FILT_REG_SINGLE_CYCLE) {
		if ((reg_temp & AD7124_FILT_REG_FILTER(7)) ==
		    AD7124_FILT_REG_FILTER(0))
			*flt_coff *= 4;
		if ((reg_temp & AD7124_FILT_REG_FILTER(7)) ==
		    AD7124_FILT_REG_FILTER(2))
			*flt_coff *= 3;
	}
	if ((reg_temp & AD7124_FILT_REG_FILTER(7)) ==
	    AD7124_FILT_REG_FILTER(4)) {
		if (power_mode == 0)
			*flt_coff *= 11;
		else
			*flt_coff *= 19;
	}
	if ((reg_temp & AD7124_FILT_REG_FILTER(7)) ==
	    AD7124_FILT_REG_FILTER(5)) {
		if (power_mode == 0)
			*flt_coff *= 10;
		else
			*flt_coff *= 18;
	}

	return 0;
}

/***************************************************************************//**
 * @brief Calculate ODR of the device.
 * @param [in] dev - Pointer to the application handler.
 * @param [in] chn_num - Channel number.
 * @return Output data rate in case of success, negative
 *         error code otherwise.
******************************************************************************/
float ad7124_get_odr(struct ad7124_dev *dev, int16_t chn_num)
{
	float f_clk;
	uint16_t fs_value, flt_coff;
	int32_t ret;
	uint32_t reg_temp;

	ret = ad7124_fclk_get(dev, &f_clk);
	if (ret)
		return ret;

	ret = ad7124_read_register2(dev,
				    (AD7124_Filter_0 + chn_num),
				    &reg_temp);
	if (ret)
		return ret;

	fs_value = reg_temp & AD7124_FILT_REG_FS(0x7FF);

	if ((reg_temp & AD7124_FILT_REG_FILTER(7)) ==
	    AD7124_FILT_REG_FILTER(7)) {
		switch ((reg_temp & AD7124_FILT_REG_POST_FILTER(7)) >> 17) {
		case 2:
			return 27.27;
		case 3:
			return 25;
		case 5:
			return 20;
		case 6:
			return 16.7;
		default:
			return -1;
		}
	}

	ret = ad7124_fltcoff_get(dev, chn_num, &flt_coff);
	if (ret)
		return ret;

	return (f_clk / (float)(flt_coff * fs_value));
}

/***************************************************************************//**
 * @brief Set ODR of the device.
 * @param [in] dev - Pointer to the application handler.
 * @param [in] odr - New ODR of the device.
 * @param [in] chn_num - Channel number.
 * @return Returns 0 for success or negative error code otherwise.
*******************************************************************************/
int32_t ad7124_set_odr(struct ad7124_dev *dev,
		       float odr,
		       int16_t chn_num)
{
	float f_clk;
	uint16_t flt_coff, fs_value;
	int32_t ret;
	uint32_t reg_temp;

	ret = ad7124_fclk_get(dev, &f_clk);
	if (ret)
		return ret;

	ret = ad7124_fltcoff_get(dev, chn_num, &flt_coff);
	if (ret)
		return ret;

	fs_value = (uint16_t)(f_clk / (flt_coff * odr));
	if (fs_value == 0)
		fs_value = 1;
	if (fs_value > 2047)
		fs_value = 2047;

	ret = ad7124_read_register2(dev,
				    (AD7124_Filter_0 + chn_num),
				    &reg_temp);
	if (ret)
		return ret;

	reg_temp &= ~AD7124_FILT_REG_FS(0x7FF);
	reg_temp |= AD7124_FILT_REG_FS(fs_value);

	return ad7124_write_register2(dev, (AD7124_Filter_0 + chn_num), reg_temp);
}

/***************************************************************************//**
 * @brief		   - SPI internal register write to device using a mask.
 * @param dev      - The device structure.
 * @param reg_addr - The register address.
 * @param data     - The register data.
 * @param mask     - The mask.
 * @return Returns 0 for success or negative error code otherwise.
*******************************************************************************/
int ad7124_reg_write_msk(struct ad7124_dev *dev,
			 uint32_t reg_addr,
			 uint32_t data,
			 uint32_t mask)
{
	int ret;
	uint32_t reg_data;

	ret = ad7124_read_register2(dev, reg_addr, &reg_data);
	if (ret)
		return ret;

	reg_data &= ~mask;
	reg_data |= data;

	return ad7124_write_register2(dev, reg_addr, reg_data);
}

/***************************************************************************//**
 * @brief Set ADC Mode
 * @param device - AD7124 Device Descriptor
 * @param adc_mode - ADC Mode to be configured
 * @return Returns 0 for success or negative error code otherwise.
******************************************************************************/
int ad7124_set_adc_mode(struct ad7124_dev *device, enum ad7124_mode adc_mode)
{
	int ret;

	if (!device || adc_mode >= ADC_MAX_MODES)
		return -EINVAL;

	ret = ad7124_reg_write_msk(device,
				   AD7124_ADC_CTRL_REG,
				   no_os_field_prep(AD7124_ADC_CTRL_REG_MODE_MSK, adc_mode),
				   AD7124_ADC_CTRL_REG_MODE_MSK);
	if (ret)
		return ret;

	device->mode = adc_mode;

	return 0;
}

/***************************************************************************//**
 * Enable/disable channel.
 * @param device - The device structure.
 * @param chn_num - The channel number.
 * @param channel_status - Channel status.
 * @return Returns 0 for success or negative error code otherwise.
*******************************************************************************/
int ad7124_set_channel_status(struct ad7124_dev *device,
			      uint8_t chn_num,
			      bool channel_status)
{
	int ret;
	uint16_t status;

	if (channel_status)
		status = AD7124_CH_MAP_REG_CH_ENABLE;
	else
		status = 0x0U;

	ret = ad7124_reg_write_msk(device,
				   AD7124_CH0_MAP_REG+chn_num,
				   status,
				   AD7124_CH_MAP_REG_CH_ENABLE);
	if (ret)
		return ret;

	device->chan_map[chn_num].channel_enable = channel_status;

	return 0;
}

/***************************************************************************//**
 * @brief Set Analog Inputs to channel.
 * @param device - AD7124 Device Descriptor.
 * @param chn_num - Channel whose Analog input is to be configured.
 * @param analog_input - Analog Inputs to the Channel.
 * @return Returns 0 for success or negative error code otherwise.
*****************************************************************************/
int ad7124_connect_analog_input(struct ad7124_dev *device,
				uint8_t chn_num,
				struct ad7124_analog_inputs analog_input)
{
	int ret;

	/* Select the Positive Analog Input */
	ret = ad7124_reg_write_msk(device,
				   AD7124_CH0_MAP_REG+chn_num,
				   no_os_field_prep(AD7124_CHMAP_REG_AINPOS_MSK, analog_input.ainp),
				   AD7124_CHMAP_REG_AINPOS_MSK);
	if (ret)
		return ret;

	/* Select the Negative Analog Input */
	ret = ad7124_reg_write_msk(device,
				   AD7124_CH0_MAP_REG+chn_num,
				   no_os_field_prep(AD7124_CHMAP_REG_AINNEG_MSK, analog_input.ainm),
				   AD7124_CHMAP_REG_AINNEG_MSK);
	if (ret)
		return ret;

	device->chan_map[chn_num].ain.ainp =
		analog_input.ainp;
	device->chan_map[chn_num].ain.ainm =
		analog_input.ainm;

	return 0;
}

/***************************************************************************//**
 * @brief Assign Setup to Channel.
 * @param device - AD7124 Device Descriptor.
 * @param chn_num - Channel ID (number).
 * @param setup - Setup ID (number).
 * @return Returns 0 for success or negative error code otherwise.
******************************************************************************/
int ad7124_assign_setup(struct ad7124_dev *device,
			uint8_t chn_num,
			uint8_t setup)
{
	int ret;

	/* Assign setup to the Channel Register. */
	ret = ad7124_reg_write_msk(device,
				   AD7124_CH0_MAP_REG+chn_num,
				   no_os_field_prep(AD7124_CHMAP_REG_SETUP_SEL_MSK, setup),
				   AD7124_CHMAP_REG_SETUP_SEL_MSK);
	if (ret)
		return (ret);

	device->chan_map[chn_num].setup_sel = setup;

	return 0;
}

/***************************************************************************//**
 * @brief Set Polarity
 * @param device - AD7124 Device Descriptor.
 * @param bipolar - Polarity Select:True in case of Bipolar,
 *					False in case of Unipolar.
 * @param setup_id - Setup ID (number).
 * @return Returns 0 for success or negative error code otherwise.
*****************************************************************************/
int ad7124_set_polarity(struct ad7124_dev* device,
			bool bipolar,
			uint8_t setup_id)
{
	int ret;
	uint32_t reg_data;

	if (bipolar)
		reg_data = AD7124_CFG_REG_BIPOLAR;
	else
		reg_data = 0x0U;

	ret = ad7124_reg_write_msk(device,
				   AD7124_CFG0_REG+setup_id,
				   reg_data,
				   AD7124_CFG_REG_BIPOLAR);
	if (ret)
		return ret;

	device->setups[setup_id].bi_unipolar = bipolar;

	return 0;
}

/***************************************************************************//*
 * @brief Select the reference source.
 * @param device - AD7124 Device Descriptor.
 * @param ref_source - Reference source.
 * @param setup_id - Setup ID (Number).
 * @return Returns 0 for success or negative error code otherwise.
******************************************************************************/
int ad7124_set_reference_source(struct ad7124_dev* device,
				enum ad7124_reference_source ref_source,
				uint8_t setup_id,
				bool ref_en)
{
	int ret;
	uint16_t status;

	if (!device || ref_source >= MAX_REF_SOURCES)
		return -EINVAL;

	ret = ad7124_reg_write_msk(device,
				   AD7124_CFG0_REG+setup_id,
				   no_os_field_prep(AD7124_SETUP_CONF_REG_REF_SEL_MSK, ref_source),
				   AD7124_SETUP_CONF_REG_REF_SEL_MSK);
	if (ret)
		return ret;

	device->setups[setup_id].ref_source = ref_source;

	if (ref_en)
		status = AD7124_ADC_CTRL_REG_REF_EN;
	else
		status = 0x0U;

	/* Enable the REF_EN Bit in case of Internal reference */
	if (ref_source == INTERNAL_REF) {
		ret = ad7124_reg_write_msk(device,
					   AD7124_ADC_CTRL_REG,
					   status,
					   AD7124_ADC_CTRL_REG_REF_EN);
		if (ret)
			return ret;
	}

	device->ref_en = ref_en;

	return 0;
}

/***************************************************************************//**
 * @brief Enable Input Buffer.
 * @param device - AD7124 Device Descriptor.
 * @param inbuf_en - Enable Input Buffer.
 * @param refbuf_en - Enable reference Buffer.
 * @param setup_id - Setup ID (Number).
 * @return Returns 0 for success or negative error code otherwise.
******************************************************************************/
int ad7124_enable_buffers(struct ad7124_dev* device,
			  bool inbuf_en,
			  bool refbuf_en,
			  uint8_t setup_id)
{
	int ret;
	uint32_t reg_val;

	if (inbuf_en)
		/* Enable input buffer for the chosen set up. */
		reg_val = (AD7124_CFG_REG_AIN_BUFP |
			   AD7124_CFG_REG_AINN_BUFM);
	else
		reg_val =  0;

	ret = ad7124_reg_write_msk(device,
				   AD7124_CFG0_REG+setup_id,
				   reg_val,
				   AD7124_AIN_BUF_MSK);
	if (ret)
		return ret;

	if (refbuf_en)
		/* Enable reference buffer for the chosen set up */
		reg_val = (AD7124_CFG_REG_REF_BUFP |
			   AD7124_CFG_REG_REF_BUFM);
	else
		reg_val = 0;

	ret = ad7124_reg_write_msk(device,
				   AD7124_CFG0_REG+setup_id,
				   reg_val,
				   AD7124_REF_BUF_MSK);
	if (ret)
		return ret;

	device->setups[setup_id].ain_buff = inbuf_en;
	device->setups[setup_id].ref_buff = refbuf_en;

	return 0;
}

/***************************************************************************//**
 * @brief Select the Power Mode.
 * @param device - AD7124 Device Descriptor.
 * @param mode - ADC Power Mode.
 * @return Returns 0 for success or negative error code otherwise.
******************************************************************************/
int ad7124_set_power_mode(struct ad7124_dev *device,
			  enum ad7124_power_mode mode)
{
	int ret;

	ret = ad7124_reg_write_msk(device,
				   AD7124_ADC_CTRL_REG,
				   no_os_field_prep(AD7124_POWER_MODE_MSK, mode),
				   AD7124_POWER_MODE_MSK);
	if (ret)
		return ret;

	device->power_mode = mode;

	return 0;
}

/***************************************************************************//**
 * @brief Initializes the AD7124.
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		               parameters.
 * @return Returns 0 for success or negative error code otherwise.
*******************************************************************************/
int32_t ad7124_setup(struct ad7124_dev **device,
		     struct ad7124_init_param *init_param)
{
	int32_t ret;
	struct ad7124_dev *dev;
	uint8_t setup_index;
	uint8_t ch_index;

	dev = (struct ad7124_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->regs = init_param->regs;
	dev->spi_rdy_poll_cnt = init_param->spi_rdy_poll_cnt;

	/* Initialize the SPI communication. */
	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret)
		goto error_dev;

	/* Update the device structure with power-on/reset settings. */
	dev->check_ready = init_param->check_ready;

	/*  Reset the device interface.*/
	ret = ad7124_reset(dev);
	if (ret)
		goto error_spi;

	/* Initialize ADC mode register. */
	ret = ad7124_write_register(dev, dev->regs[AD7124_ADC_CTRL_REG]);
	if (ret)
		goto error_spi;

	/* Get CRC State. */
	ad7124_update_crcsetting(dev);
	ad7124_update_dev_spi_settings(dev);

	dev->active_device = init_param->active_device;

	/* Read ID register to identify the part. */
	ret = ad7124_read_register(dev, &dev->regs[AD7124_ID_REG]);
	if (ret)
		goto error_spi;

	if (dev->active_device == ID_AD7124_4) {
		switch (dev->regs[AD7124_ID_REG].value) {
		case AD7124_4_STD_ID:
		case AD7124_4_B_GRADE_ID:
		case AD7124_4_NEW_ID:
			break;

		default:
			goto error_spi;
		}
	}

	else if (dev->active_device == ID_AD7124_8) {
		switch (dev->regs[AD7124_ID_REG].value) {
		case AD7124_8_STD_ID:
		case AD7124_8_B_W_GRADE_ID:
		case AD7124_8_NEW_ID:
			break;

		default:
			goto error_spi;
		}
	}

	for (setup_index = 0; setup_index < AD7124_MAX_SETUPS; setup_index++) {
		ret = ad7124_set_polarity(dev,
					  init_param->setups[setup_index].bi_unipolar,
					  setup_index);
		if (ret)
			goto error_spi;

		ret = ad7124_set_reference_source(dev,
						  init_param->setups[setup_index].ref_source,
						  setup_index,
						  init_param->ref_en);
		if (ret)
			goto error_spi;

		ret = ad7124_enable_buffers(dev,
					    init_param->setups[setup_index].ain_buff,
					    init_param->setups[setup_index].ref_buff,
					    setup_index);
		if (ret)
			goto error_spi;
	}

	ret = ad7124_set_adc_mode(dev, init_param->mode);
	if (ret)
		goto error_spi;

	ret = ad7124_set_power_mode(dev,
				    init_param->power_mode);
	if (ret)
		goto error_spi;

	for (ch_index = 0; ch_index < AD7124_MAX_CHANNELS; ch_index++) {
		ret = ad7124_connect_analog_input(dev,
						  ch_index,
						  init_param->chan_map[ch_index].ain);
		if (ret)
			goto error_spi;

		ret = ad7124_assign_setup(dev,
					  ch_index,
					  init_param->chan_map[ch_index].setup_sel);
		if (ret)
			goto error_spi;

		ret = ad7124_set_channel_status(dev,
						ch_index,
						init_param->chan_map[ch_index].channel_enable);
		if (ret)
			goto error_spi;
	}

	*device = dev;

	return 0;

error_spi:
	no_os_spi_remove(dev->spi_desc);
error_dev:
	free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad7124_setup().
 * @param dev - The device structure.
 * @return Returns 0 for success or negative error code otherwise.
*******************************************************************************/
int32_t ad7124_remove(struct ad7124_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}
