/***************************************************************************//**
*   @file    ad7124.c
*   @brief   AD7124 implementation file.
*   	     Devices: AD7124-4, AD7124-8
*
********************************************************************************
* Copyright 2015-2019(c) Analog Devices, Inc.
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
#include "delay.h"

/* Error codes */
#define INVALID_VAL -1 /* Invalid argument */
#define COMM_ERR    -2 /* Communication error on receive */
#define TIMEOUT     -3 /* A timeout has occured */

/*
 * Post reset delay required to ensure all internal config done
 * A time of 2ms should be enough based on the data sheet, but 4ms
 * chosen to provide enough margin, in case mdelay is not accurate.
 */
#define AD7124_POST_RESET_DELAY      4


/***************************************************************************//**
 * @brief Reads the value of the specified register without checking if the
 *        device is ready to accept user requests.
 *
 * @param dev   - The handler of the instance of the driver.
 * @param p_reg - Pointer to the register structure holding info about the
 *               register to be read. The read value is stored inside the
 *               register structure.
 *
 * @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t ad7124_no_check_read_register(struct ad7124_dev *dev,
				      struct ad7124_st_reg* p_reg)
{
	int32_t ret = 0;
	uint8_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t i = 0;
	uint8_t check8 = 0, add_status_length = 0;
	uint8_t msg_buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	if(!dev || !p_reg)
		return INVALID_VAL;

	/* Build the Command word */
	buffer[0] = AD7124_COMM_REG_WEN | AD7124_COMM_REG_RD |
		    AD7124_COMM_REG_RA(p_reg->addr);

	/*
	 * If this is an AD7124_DATA register read, and the DATA_STATUS bit is set
	 * in ADC_CONTROL, need to read 4, not 3 bytes for DATA with STATUS
	 */
	if ((p_reg->addr == AD7124_DATA_REG) &&
	    (dev->regs[AD7124_ADC_Control].value & AD7124_ADC_CTRL_REG_DATA_STATUS)) {
		add_status_length = 1;
	}

	/* Read data from the device */
	ret = spi_write_and_read(dev->spi_desc,
				 buffer,
				 ((dev->use_crc != AD7124_DISABLE_CRC) ? p_reg->size + 1
				  : p_reg->size) + 1 + add_status_length);
	if(ret < 0)
		return ret;

	/* Check the CRC */
	if(dev->use_crc == AD7124_USE_CRC) {
		msg_buf[0] = AD7124_COMM_REG_WEN | AD7124_COMM_REG_RD |
			     AD7124_COMM_REG_RA(p_reg->addr);
		for(i = 1; i < p_reg->size + 2 + add_status_length; ++i) {
			msg_buf[i] = buffer[i];
		}
		check8 = ad7124_compute_crc8(msg_buf, p_reg->size + 2 + add_status_length);
	}

	if(check8 != 0) {
		/* ReadRegister checksum failed. */
		return COMM_ERR;
	}

	/*
	 * if reading Data with 4 bytes, need to copy the status byte to the STATUS
	 * register struct value member
	 */
	if (add_status_length) {
		dev->regs[AD7124_Status].value = buffer[p_reg->size + 1];
	}

	/* Build the result */
	p_reg->value = 0;
	for(i = 1; i < p_reg->size + 1; i++) {
		p_reg->value <<= 8;
		p_reg->value += buffer[i];
	}

	return ret;
}

/***************************************************************************//**
 * @brief Writes the value of the specified register without checking if the
 *        device is ready to accept user requests.
 *
 * @param dev - The handler of the instance of the driver.
 * @param reg - Register structure holding info about the register to be written
 *
 * @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t ad7124_no_check_write_register(struct ad7124_dev *dev,
				       struct ad7124_st_reg reg)
{
	int32_t ret = 0;
	int32_t reg_value = 0;
	uint8_t wr_buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t i = 0;
	uint8_t crc8 = 0;

	if(!dev)
		return INVALID_VAL;

	/* Build the Command word */
	wr_buf[0] = AD7124_COMM_REG_WEN | AD7124_COMM_REG_WR |
		    AD7124_COMM_REG_RA(reg.addr);

	/* Fill the write buffer */
	reg_value = reg.value;
	for(i = 0; i < reg.size; i++) {
		wr_buf[reg.size - i] = reg_value & 0xFF;
		reg_value >>= 8;
	}

	/* Compute the CRC */
	if(dev->use_crc != AD7124_DISABLE_CRC) {
		crc8 = ad7124_compute_crc8(wr_buf, reg.size + 1);
		wr_buf[reg.size + 1] = crc8;
	}

	/* Write data to the device */
	ret = spi_write_and_read(dev->spi_desc,
				 wr_buf,
				 (dev->use_crc != AD7124_DISABLE_CRC) ? reg.size + 2
				 : reg.size + 1);

	return ret;
}

/***************************************************************************//**
 * @brief Reads the value of the specified register only when the device is ready
 *        to accept user requests. If the device ready flag is deactivated the
 *        read operation will be executed without checking the device state.
 *        DEPRECATED, use ad7124_read_register2.
 *
 * @param dev   - The handler of the instance of the driver.
 * @param p_reg - Pointer to the register structure holding info about the
 *               register to be read. The read value is stored inside the
 *               register structure.
 *
 * @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t ad7124_read_register(struct ad7124_dev *dev,
			     struct ad7124_st_reg* p_reg)
{
	int32_t ret;

	if (p_reg->addr != AD7124_ERR_REG && dev->check_ready) {
		ret = ad7124_wait_for_spi_ready(dev,
						dev->spi_rdy_poll_cnt);
		if (ret < 0)
			return ret;
	}
	ret = ad7124_no_check_read_register(dev,
					    p_reg);

	return ret;
}

/**
 * @brief Wrap the read register function to give it a modern signature.
 * @param [in] dev - Driver handler pointer.
 * @param [in] reg - Address of the register to be read.
 * @param [out] readval - Pointer to the register value.
 * @return SUCCESS in case of success, error code otherwise.
 */
int32_t ad7124_read_register2(struct ad7124_dev *dev, uint32_t reg,
			      uint32_t *readval)
{
	int32_t ret;

	ret = ad7124_read_register(dev, &dev->regs[reg]);
	if (ret != 0)
		return ret;

	*readval = dev->regs[reg].value;

	return ret;
}

/***************************************************************************//**
 * @brief Writes the value of the specified register only when the device is
 *        ready to accept user requests. If the device ready flag is deactivated
 *        the write operation will be executed without checking the device state.
 *        DEPRECATED, use ad7124_write_register2.
 *
 * @param dev - The handler of the instance of the driver.
 * @param p_reg - Register structure holding info about the register to be written
 *
 * @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t ad7124_write_register(struct ad7124_dev *dev,
			      struct ad7124_st_reg p_reg)
{
	int32_t ret;

	if (dev->check_ready) {
		ret = ad7124_wait_for_spi_ready(dev,
						dev->spi_rdy_poll_cnt);
		if (ret < 0)
			return ret;
	}
	ret = ad7124_no_check_write_register(dev,
					     p_reg);

	return ret;
}

/**
 * @brief Wrap the write register function to give it a modern signature.
 * @param [in] dev - Driver handler pointer.
 * @param [in] reg - Address of the register to be read.
 * @param [in] writeval - New value for the register.
 * @return SUCCESS in case of success, error code otherwise.
 */
int32_t ad7124_write_register2(struct ad7124_dev *dev, uint32_t reg,
			       uint32_t writeval)
{
	dev->regs[reg].value = writeval;

	return ad7124_write_register(dev, dev->regs[reg]);
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @param dev - The handler of the instance of the driver.
 *
 * @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t ad7124_reset(struct ad7124_dev *dev)
{
	int32_t ret = 0;
	uint8_t wr_buf[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	if(!dev)
		return INVALID_VAL;

	ret = spi_write_and_read(dev->spi_desc,
				 wr_buf,
				 8);

	/* CRC is disabled after reset */
	dev->use_crc = AD7124_DISABLE_CRC;

	/* Read POR bit to clear */
	ret = ad7124_wait_to_power_on(dev,
				      dev->spi_rdy_poll_cnt);

	mdelay(AD7124_POST_RESET_DELAY);

	return ret;
}

/***************************************************************************//**
 * @brief Waits until the device can accept read and write user actions.
 *
 * @param dev     - The handler of the instance of the driver.
 * @param timeout - Count representing the number of polls to be done until the
 *                  function returns.
 *
 * @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t ad7124_wait_for_spi_ready(struct ad7124_dev *dev,
				  uint32_t timeout)
{
	struct ad7124_st_reg *regs;
	int32_t ret;
	int8_t ready = 0;

	if(!dev)
		return INVALID_VAL;

	regs = dev->regs;

	while(!ready && --timeout) {
		/* Read the value of the Error Register */
		ret = ad7124_read_register(dev, &regs[AD7124_Error]);
		if(ret < 0)
			return ret;

		/* Check the SPI IGNORE Error bit in the Error Register */
		ready = (regs[AD7124_Error].value &
			 AD7124_ERR_REG_SPI_IGNORE_ERR) == 0;
	}

	return timeout ? 0 : TIMEOUT;
}

/***************************************************************************//**
 * @brief Waits until the device finishes the power-on reset operation.
 *
 * @param dev     - The handler of the instance of the driver.
 * @param timeout - Count representing the number of polls to be done until the
 *                  function returns.
 *
 * @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t ad7124_wait_to_power_on(struct ad7124_dev *dev,
				uint32_t timeout)
{
	struct ad7124_st_reg *regs;
	int32_t ret;
	int8_t powered_on = 0;

	if(!dev)
		return INVALID_VAL;

	regs = dev->regs;

	while(!powered_on && timeout--) {
		ret = ad7124_read_register(dev,
					   &regs[AD7124_Status]);
		if(ret < 0)
			return ret;

		/* Check the POR_FLAG bit in the Status Register */
		powered_on = (regs[AD7124_Status].value &
			      AD7124_STATUS_REG_POR_FLAG) == 0;
	}

	return (timeout || powered_on) ? 0 : TIMEOUT;
}

/***************************************************************************//**
 * @brief Waits until a new conversion result is available.
 *
 * @param dev     - The handler of the instance of the driver.
 * @param timeout - Count representing the number of polls to be done until the
 *                  function returns if no new data is available.
 *
 * @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t ad7124_wait_for_conv_ready(struct ad7124_dev *dev,
				   uint32_t timeout)
{
	struct ad7124_st_reg *regs;
	int32_t ret;
	int8_t ready = 0;

	if(!dev)
		return INVALID_VAL;

	regs = dev->regs;

	while(!ready && --timeout) {
		/* Read the value of the Status Register */
		ret = ad7124_read_register(dev, &regs[AD7124_Status]);
		if(ret < 0)
			return ret;

		/* Check the RDY bit in the Status Register */
		ready = (regs[AD7124_Status].value &
			 AD7124_STATUS_REG_RDY) == 0;
	}

	return timeout ? 0 : TIMEOUT;
}

/***************************************************************************//**
 * @brief Reads the conversion result from the device.
 *
 * @param dev     - The handler of the instance of the driver.
 * @param p_data  - Pointer to store the read data.
 *
 * @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t ad7124_read_data(struct ad7124_dev *dev,
			 int32_t* p_data)
{
	struct ad7124_st_reg *regs;
	int32_t ret;

	if(!dev)
		return INVALID_VAL;

	regs = dev->regs;

	/* Read the value of the Status Register */
	ret = ad7124_read_register(dev, &regs[AD7124_Data]);

	/* Get the read result */
	*p_data = regs[AD7124_Data].value;

	return ret;
}

/**
 * @brief Get the ID of the channel of the latest conversion.
 *
 * @param dev     - The handler of the instance of the driver.
 * @param status  - Pointer to store the read data.
 *
 * @return Returns 0 for success or negative error code.
 */
int32_t ad7124_get_read_chan_id(struct ad7124_dev *dev, uint32_t *status)
{
	int32_t ret;
	uint32_t reg_temp;

	ret = ad7124_read_register2(dev, AD7124_STATUS_REG, &reg_temp);
	if (ret != 0)
		return ret;
	*status = reg_temp & AD7124_STATUS_REG_CH_ACTIVE(0xF);

	return ret;
}

/***************************************************************************//**
 * @brief Computes the CRC checksum for a data buffer.
 *
 * @param p_buf    - Data buffer
 * @param buf_size - Data buffer size in bytes
 *
 * @return Returns the computed CRC checksum.
*******************************************************************************/
uint8_t ad7124_compute_crc8(uint8_t * p_buf, uint8_t buf_size)
{
	uint8_t i = 0;
	uint8_t crc = 0;

	while(buf_size) {
		for(i = 0x80; i != 0; i >>= 1) {
			bool cmp1 = (crc & 0x80) != 0;
			bool cmp2 = (*p_buf & i) != 0;
			if(cmp1 != cmp2) { /* MSB of CRC register XOR input Bit from Data */
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
 *
 * @param dev - The handler of the instance of the driver.
 *
 * @return None.
*******************************************************************************/
void ad7124_update_crcsetting(struct ad7124_dev *dev)
{
	struct ad7124_st_reg *regs;

	if(!dev)
		return;

	regs = dev->regs;

	/* Get CRC State. */
	if (regs[AD7124_Error_En].value & AD7124_ERREN_REG_SPI_CRC_ERR_EN) {
		dev->use_crc = AD7124_USE_CRC;
	} else {
		dev->use_crc = AD7124_DISABLE_CRC;
	}
}

/***************************************************************************//**
 * @brief Updates the device SPI interface settings.
 *
 * @param dev - The handler of the instance of the driver.
 *
 * @return None.
*******************************************************************************/
void ad7124_update_dev_spi_settings(struct ad7124_dev *dev)
{
	struct ad7124_st_reg *regs;

	if(!dev)
		return;

	regs = dev->regs;

	if (regs[AD7124_Error_En].value & AD7124_ERREN_REG_SPI_IGNORE_ERR_EN) {
		dev->check_ready = 1;
	} else {
		dev->check_ready = 0;
	}
}

/***************************************************************************//**
 * @brief Initializes the AD7124.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t ad7124_setup(struct ad7124_dev **device,
		     struct ad7124_init_param *init_param)
{
	int32_t ret;
	enum ad7124_registers reg_nr;
	struct ad7124_dev *dev;

	dev = (struct ad7124_dev *)malloc(sizeof(*dev));
	if (!dev)
		return INVALID_VAL;

	dev->regs = init_param->regs;
	dev->spi_rdy_poll_cnt = init_param->spi_rdy_poll_cnt;

	/* Initialize the SPI communication. */
	ret = spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret < 0)
		return ret;

	/*  Reset the device interface.*/
	ret = ad7124_reset(dev);
	if (ret < 0)
		return ret;

	/* Update the device structure with power-on/reset settings */
	dev->check_ready = 1;

	/* Initialize registers AD7124_ADC_Control through AD7124_Filter_7. */
	for(reg_nr = AD7124_Status; (reg_nr < AD7124_Offset_0) && !(ret < 0);
	    reg_nr++) {
		if (dev->regs[reg_nr].rw == AD7124_RW) {
			ret = ad7124_write_register(dev, dev->regs[reg_nr]);
			if (ret < 0)
				break;
		}

		/* Get CRC State and device SPI interface settings */
		if (reg_nr == AD7124_Error_En) {
			ad7124_update_crcsetting(dev);
			ad7124_update_dev_spi_settings(dev);
		}
	}

	*device = dev;

	return ret;
}

/**
 * @brief Get the AD7124 reference clock.
 * @param [in] dev - Pointer to the application handler.
 * @param [out] f_clk - Pointer to the clock frequency container.
 * @return 0 in case of success, error code otherwise.
 */
int32_t ad7124_fclk_get(struct ad7124_dev *dev, float *f_clk)
{
	int32_t ret;
	const float	f_clk_fp = 614400,
			f_clk_mp = 153600,
			f_clk_lp = 76800;
	uint32_t reg_temp;

	ret = ad7124_read_register2(dev, AD7124_ADC_Control, &reg_temp);
	if (ret != 0)
		return ret;

	switch ((reg_temp & AD7124_ADC_CTRL_REG_POWER_MODE(3)) >> 6) {
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

	return ret;
}

/**
 * @brief Get the filter coefficient for the sample rate.
 * @param [in] dev - Pointer to the application handler.
 * @param [in] ch_no - Channel number.
 * @param [out] flt_coff - Pointer to the filter coefficient container.
 * @return 0 in case of success, error code otherwise.
 */
int32_t ad7124_fltcoff_get(struct ad7124_dev *dev, int16_t ch_no,
			   uint16_t *flt_coff)
{
	uint16_t power_mode;
	int32_t ret;
	uint32_t reg_temp;

	ret = ad7124_read_register2(dev, AD7124_ADC_Control, &reg_temp);
	if (ret != 0)
		return ret;

	power_mode = (reg_temp & AD7124_ADC_CTRL_REG_POWER_MODE(3)) >> 6;

	ret = ad7124_read_register2(dev, (AD7124_Filter_0 + ch_no), &reg_temp);
	if (ret != 0)
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

	return ret;
}

/**
 * @brief Calculate ODR of the device.
 * @param [in] dev - Pointer to the application handler.
 * @param [in] ch_no - Channel number.
 * @return Output data rate in case of success, negative
 *         error code otherwise.
 */
float ad7124_get_odr(struct ad7124_dev *dev, int16_t ch_no)
{
	float f_clk;
	uint16_t fs_value, flt_coff;
	int32_t ret;
	uint32_t reg_temp;

	ret = ad7124_fclk_get(dev, &f_clk);
	if (ret != 0)
		return ret;

	ret = ad7124_read_register2(dev, (AD7124_Filter_0 + ch_no),
				    &reg_temp);
	if (ret != 0)
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

	ret = ad7124_fltcoff_get(dev, ch_no, &flt_coff);
	if (ret != 0)
		return ret;

	return (f_clk / (float)(flt_coff * fs_value));
}

/**
 * @brief Set ODR of the device.
 * @param [in] dev - Pointer to the application handler.
 * @param [in] odr - New ODR of the device.
 * @param [in] ch_no - Channel number.
 * @return 0 in case of success, error code otherwise.
 */
int32_t ad7124_set_odr(struct ad7124_dev *dev, float odr,
		       int16_t ch_no)
{
	float f_clk;
	uint16_t flt_coff, fs_value;
	int32_t ret;
	uint32_t reg_temp;

	ret = ad7124_fclk_get(dev, &f_clk);
	if (ret != 0)
		return ret;

	ret = ad7124_fltcoff_get(dev, ch_no, &flt_coff);
	if (ret != 0)
		return ret;

	fs_value = (uint16_t)(f_clk / (flt_coff * odr));
	if (fs_value == 0)
		fs_value = 1;
	if (fs_value > 2047)
		fs_value = 2047;

	ret = ad7124_read_register2(dev, (AD7124_Filter_0 + ch_no),
				    &reg_temp);
	if (ret != 0)
		return ret;
	reg_temp &= ~AD7124_FILT_REG_FS(0x7FF);
	reg_temp |= AD7124_FILT_REG_FS(fs_value);

	ret = ad7124_write_register2(dev, (AD7124_Filter_0 + ch_no),
				     reg_temp);
	if (ret != 0)
		return ret;

	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by AD7124_Setup().
 *
 * @param dev - The device structure.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad7124_remove(struct ad7124_dev *dev)
{
	int32_t ret;

	ret = spi_remove(dev->spi_desc);

	free(dev);

	return ret;
}
