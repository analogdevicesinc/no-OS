/***************************************************************************//**
*   @file    AD717X.c
*   @brief   AD717X implementation file.
*   	     Devices: AD7172-2, AD7172-4, AD7173-8, AD7175-2, AD7175-8, AD7176-2
*            AD7177-2, AD4111, AD4112, AD4114, AD4115, AD4116
*   @author  acozma (andrei.cozma@analog.com)
*            dnechita (dan.nechita@analog.com)
*
********************************************************************************
* Copyright 2015(c) Analog Devices, Inc.
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
#include "ad717x.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/* Error codes */
#define INVALID_VAL -1 /* Invalid argument */
#define COMM_ERR    -2 /* Communication error on receive */
#define TIMEOUT     -3 /* A timeout has occured */

/***************************************************************************//**
 * @brief Set channel status - Enable/Disable
 * @param device - AD717x Device descriptor.
 * @param channel_id - Channel ID (number) of the channel whose status is to be set.
 * @param channel_status - Required status of the channel-True in case of Enable
 *			    	and False in case of Disable
 * @return Returns 0 for success or negative error code in case of failure.
*******************************************************************************/
int ad717x_set_channel_status(ad717x_dev *device, uint8_t channel_id,
			      bool channel_status)
{
	ad717x_st_reg *chn_register;
	int ret;

	if (!device)
		return -EINVAL;

	/* Point to the Channel register */
	chn_register = AD717X_GetReg(device, AD717X_CHMAP0_REG + channel_id);
	if (!chn_register)
		return -EINVAL;

	if (channel_status)
		/* Assign the Channel enable bit and write to channel register */
		chn_register->value |= AD717X_CHMAP_REG_CH_EN;
	else
		chn_register->value &= ~(AD717X_CHMAP_REG_CH_EN);

	ret = AD717X_WriteRegister(device, AD717X_CHMAP0_REG + channel_id);
	if (ret < 0)
		return ret;
	device->chan_map[channel_id].channel_enable = channel_status;

	return 0;
}

/***************************************************************************//**
 * @brief Set ADC Mode
 * @param device - AD717x Device Descriptor
 * @param adc_mode - ADC Mode to be configured
 * @return Returns 0 for success or negative error code in case of failure.
******************************************************************************/
int ad717x_set_adc_mode(ad717x_dev *device, enum ad717x_mode adc_mode)
{
	ad717x_st_reg *adc_mode_reg;

	if (!device)
		return -EINVAL;

	/* Retrieve the ADC Mode reigster */
	adc_mode_reg = AD717X_GetReg(device, AD717X_ADCMODE_REG);
	if (!adc_mode_reg)
		return -EINVAL;

	/* Clear the Mode[6:4] bits in the ADC Mode Register */
	adc_mode_reg->value &= ~(AD717X_ADCMODE_REG_MODE_MSK);

	/* Set the required conversion mode, write to register */
	adc_mode_reg->value |= AD717X_ADCMODE_REG_MODE(adc_mode);
	if (AD717X_WriteRegister(device, AD717X_ADCMODE_REG) < 0)
		return -EINVAL;
	device->mode = adc_mode;

	return 0;
}

/***************************************************************************//**
 * @brief Set Analog Inputs to channel
 * @param device - AD717x Device Descriptor
 * @param channel_id - Channel whose Analog input is to be configured
 * @param analog_input - Analog Inputs to the Channel
 * @return Returns 0 for success or negative error code in case of failure.
*****************************************************************************/
int ad717x_connect_analog_input(ad717x_dev *device, uint8_t channel_id,
				union ad717x_analog_inputs analog_input)
{
	ad717x_st_reg *channel_reg;

	if (!device)
		return -EINVAL;

	/* Retrieve the channel register */
	channel_reg = AD717X_GetReg(device, AD717X_CHMAP0_REG + channel_id);
	if (!channel_reg)
		return -EINVAL;

	switch ((uint8_t)device->active_device) {
	case ID_AD4111 :
	case ID_AD4112 :
	case ID_AD4114 :
	case ID_AD4115 :
	case ID_AD4116 :
		/* Clear and Set the required analog input pair to channel */
		channel_reg->value  &= ~AD717x_CHANNEL_INPUT_MASK;
		channel_reg->value |= AD4111_CHMAP_REG_INPUT(analog_input.analog_input_pairs);
		if (AD717X_WriteRegister(device, AD717X_CHMAP0_REG + channel_id) < 0)
			return -EINVAL;

		device->chan_map[channel_id].analog_inputs.analog_input_pairs =
			analog_input.analog_input_pairs;
		break;

	case ID_AD7172_4:
	case ID_AD7173_8:
	case ID_AD7175_2:
	case ID_AD7175_8:
	case ID_AD7176_2:
	case ID_AD7177_2:
	case ID_AD7172_2:
		/* Select the Positive Analog Input */
		channel_reg->value &= ~AD717X_CHMAP_REG_AINPOS_MSK;
		channel_reg->value |=  AD717X_CHMAP_REG_AINPOS(
					       analog_input.ainp.pos_analog_input);

		/* Select the Negative Analog Input */
		channel_reg->value &= ~AD717X_CHMAP_REG_AINNEG_MSK;
		channel_reg->value |= AD717X_CHMAP_REG_AINNEG(
					      analog_input.ainp.neg_analog_input);
		if (AD717X_WriteRegister(device, AD717X_CHMAP0_REG + channel_id) < 0)
			return -EINVAL;

		device->chan_map[channel_id].analog_inputs.ainp.pos_analog_input =
			analog_input.ainp.pos_analog_input;
		device->chan_map[channel_id].analog_inputs.ainp.neg_analog_input =
			analog_input.ainp.neg_analog_input;
		break;

	default :
		return -EINVAL;
	}

	return 0;
}

/***************************************************************************//**
 * @brief Assign Setup to Channel
 * @param device - AD717x Device Descriptor
 * @param channel_id - Channel ID (number)
 * @param setup - Setup ID (number)
 * @return Returns 0 for success or negative error code in case of failure.
******************************************************************************/
int ad717x_assign_setup(ad717x_dev *device, uint8_t channel_id, uint8_t setup)
{
	ad717x_st_reg *p_register;

	if (!device)
		return -EINVAL;

	/* Retrieve the Channel Register */
	p_register = AD717X_GetReg(device, AD717X_CHMAP0_REG + channel_id);
	if (!p_register)
		return -EINVAL;

	/* Assign set up to the chosen channel */
	p_register->value &= ~AD717X_CHMAP_REG_SETUP_SEL_MSK;
	p_register->value |= AD717X_CHMAP_REG_SETUP_SEL(setup);

	if (AD717X_WriteRegister(device, AD717X_CHMAP0_REG + channel_id) < 0)
		return -EINVAL;
	device->chan_map[channel_id].setup_sel = setup;

	return 0;
}

/***************************************************************************//**
 * @brief Set Polarity
 * @param device - AD717x Device Descriptor
 * @param bipolar - Polarity Select:True in case of Bipolar, False in case of Unipolar
 * @param setup_id - Setup ID (number)
 * @return Returns 0 for success or negative error code in case of failure.
*****************************************************************************/
int ad717x_set_polarity(ad717x_dev* device, bool bipolar, uint8_t setup_id)
{
	ad717x_st_reg* setup_reg;

	if (!device)
		return -EINVAL;

	/* Retrieve the SETUPCON Register */
	setup_reg = AD717X_GetReg(device, AD717X_SETUPCON0_REG + setup_id);
	if (!setup_reg)
		return -EINVAL;

	/* Set the BI_UNIPOLAR bit in case of BIPOLAR operation */
	if (bipolar)
		setup_reg->value |= AD717X_SETUP_CONF_REG_BI_UNIPOLAR;
	else
		setup_reg->value &= ~(AD717X_SETUP_CONF_REG_BI_UNIPOLAR);

	if (AD717X_WriteRegister(device,
				 AD717X_SETUPCON0_REG + setup_id) < 0)
		return -EINVAL;
	device->setups[setup_id].bi_unipolar = bipolar;

	return 0;
}

/***************************************************************************//**
 * @brief Select the reference source
 * @param device - AD717x Device Descriptor
 * @param ref_source - Reference source
 * @param setup_id - Setup ID (Number)
 * @return Returns 0 for success or negative error code in case of failure.
******************************************************************************/
int ad717x_set_reference_source(ad717x_dev* device,
				enum ad717x_reference_source ref_source, uint8_t setup_id)
{
	ad717x_st_reg* setup_reg;
	ad717x_st_reg *adc_mode_reg;

	if (!device)
		return -EINVAL;

	/* Retrieve the SETUPCON Register */
	setup_reg = AD717X_GetReg(device, AD717X_SETUPCON0_REG + setup_id);
	if (!setup_reg)
		return -EINVAL;

	/* Choose the reference source for the selected setup */
	setup_reg->value &= ~AD717X_SETUP_CONF_REG_REF_SEL_MSK;
	setup_reg->value |= (AD717X_SETUP_CONF_REG_REF_SEL(ref_source));

	if (AD717X_WriteRegister(device,
				 AD717X_SETUPCON0_REG + setup_id) < 0)
		return -EINVAL;
	device->setups[setup_id].ref_source = ref_source;

	/* Enable the REF_EN Bit in case of Internal reference */
	if (ref_source == INTERNAL_REF) {
		/* Retrieve the ADC Mode reigster */
		adc_mode_reg = AD717X_GetReg(device, AD717X_ADCMODE_REG);
		if (!adc_mode_reg)
			return -EINVAL;

		/* Set the REF_EN Bit */
		adc_mode_reg->value |= AD717X_ADCMODE_REG_REF_EN;
		if (AD717X_WriteRegister(device, AD717X_ADCMODE_REG) < 0)
			return -EINVAL;
		device->ref_en = true;
	}

	return 0;
}

/***************************************************************************//**
 * @brief Enable Input Buffer
 * @param device - AD717x Device Descriptor
 * @param inbuf_en - Enable Inpur Buffer
 * @param refbuf_en - Enable referece Buffer
 * @param setup_id - Setup ID (Number)
 * @return Returns 0 for success or negative error code in case of failure.
******************************************************************************/
int ad717x_enable_input_buffer(ad717x_dev* device,
			       bool inbuf_en, bool refbuf_en, uint8_t setup_id)
{
	ad717x_st_reg* setup_reg;

	if (!device)
		return -EINVAL;

	/* Retrieve the SETUPCON Register */
	setup_reg = AD717X_GetReg(device, AD717X_SETUPCON0_REG + setup_id);
	if (!setup_reg)
		return -EINVAL;

	if (inbuf_en)
		/* Enable input buffer for the chosen set up */
		setup_reg->value |= (AD717X_SETUP_CONF_REG_AINBUF_P |
				     AD717X_SETUP_CONF_REG_AINBUF_N);
	else
		setup_reg->value &= (~(AD717X_SETUP_CONF_REG_AINBUF_P |
				       AD717X_SETUP_CONF_REG_AINBUF_N));
	if (refbuf_en)
		/* Enable reference buffer for the chosen set up */
		setup_reg->value |= (AD717X_SETUP_CONF_REG_REFBUF_P |
				     AD717X_SETUP_CONF_REG_REFBUF_N);
	else
		setup_reg->value &= (~(AD717X_SETUP_CONF_REG_REFBUF_P |
				       AD717X_SETUP_CONF_REG_REFBUF_N));

	if (AD717X_WriteRegister(device,
				 AD717X_SETUPCON0_REG + setup_id) < 0)
		return -EINVAL;
	device->setups[setup_id].input_buff = inbuf_en;
	device->setups[setup_id].ref_buff = refbuf_en;

	return 0;
}

/***************************************************************************//**
 * @brief Perform Single Conversion
 * @param device - AD717x Device Descriptor
 * @param id - Channel ID (number) requested
 * @param adc_raw_data ADC Raw Value
 * @return Returns 0 for success or negative error code in case of failure.
******************************************************************************/
int ad717x_single_read(ad717x_dev* device,  uint8_t id, int32_t *adc_raw_data)
{
	int ret;

	/* Enable the requested channel */
	ret = ad717x_set_channel_status(device, id, true);
	if (ret < 0)
		return ret;

	/* Set Mode to Single Conversion */
	ret = ad717x_set_adc_mode(device, SINGLE);
	if (ret < 0)
		return ret;

	/* Wait for Conversion completion */
	ret = AD717X_WaitForReady(device, AD717X_CONV_TIMEOUT);
	if (ret < 0)
		return ret;

	/* Read the data register */
	ret = AD717X_ReadData(device, adc_raw_data);
	if (ret < 0)
		return ret;

	/* Disable the current channel */
	return ad717x_set_channel_status(device, id, false);
}

/***************************************************************************//**
* @brief  Searches through the list of registers of the driver instance and
*         retrieves a pointer to the register that matches the given address.
*
* @param device - The handler of the instance of the driver.
* @param reg_address - The address to be used to find the register.
*
* @return A pointer to the register if found or 0.
*******************************************************************************/
ad717x_st_reg *AD717X_GetReg(ad717x_dev *device,
			     uint8_t reg_address)
{
	uint8_t i;
	ad717x_st_reg *reg = 0;

	if (!device || !device->regs)
		return 0;

	for (i = 0; i < device->num_regs; i++) {
		if (device->regs[i].addr == reg_address) {
			reg = &device->regs[i];
			break;
		}
	}

	return reg;
}

/***************************************************************************//**
* @brief Reads the value of the specified register.
*
* @param device - The handler of the instance of the driver.
* @param addr - The address of the register to be read. The value will be stored
*         inside the register structure that holds info about this register.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD717X_ReadRegister(ad717x_dev *device,
			    uint8_t addr)
{
	int32_t ret       = 0;
	uint8_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t i         = 0;
	uint8_t check8    = 0;
	uint8_t msgBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	ad717x_st_reg *pReg;

	if(!device)
		return INVALID_VAL;

	pReg = AD717X_GetReg(device, addr);
	if (!pReg)
		return INVALID_VAL;

	/* Build the Command word */
	buffer[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_RD |
		    AD717X_COMM_REG_RA(pReg->addr);

	/* Read data from the device */
	ret = no_os_spi_write_and_read(device->spi_desc,
				       buffer,
				       ((device->useCRC != AD717X_DISABLE) ? pReg->size + 1
					: pReg->size) + 1);
	if(ret < 0)
		return ret;

	/* Check the CRC */
	if(device->useCRC == AD717X_USE_CRC) {
		msgBuf[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_RD |
			    AD717X_COMM_REG_RA(pReg->addr);
		for(i = 1; i < pReg->size + 2; ++i) {
			msgBuf[i] = buffer[i];
		}
		check8 = AD717X_ComputeCRC8(msgBuf, pReg->size + 2);
	}
	if(device->useCRC == AD717X_USE_XOR) {
		msgBuf[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_RD |
			    AD717X_COMM_REG_RA(pReg->addr);
		for(i = 1; i < pReg->size + 2; ++i) {
			msgBuf[i] = buffer[i];
		}
		check8 = AD717X_ComputeXOR8(msgBuf, pReg->size + 2);
	}

	if(check8 != 0) {
		/* ReadRegister checksum failed. */
		return COMM_ERR;
	}

	/* Build the result */
	pReg->value = 0;
	for(i = 1; i < pReg->size + 1; i++) {
		pReg->value <<= 8;
		pReg->value += buffer[i];
	}

	return ret;
}

/***************************************************************************//**
* @brief Writes the value of the specified register.
*
* @param device - The handler of the instance of the driver.
* @param addr   - The address of the register to be written with the value stored
*               inside the register structure that holds info about this
*               register.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD717X_WriteRegister(ad717x_dev *device,
			     uint8_t addr)
{
	int32_t ret      = 0;
	int32_t regValue = 0;
	uint8_t wrBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t i        = 0;
	uint8_t crc8     = 0;
	ad717x_st_reg *preg;

	if(!device)
		return INVALID_VAL;

	preg = AD717X_GetReg(device, addr);
	if (!preg)
		return INVALID_VAL;

	/* Build the Command word */
	wrBuf[0] = AD717X_COMM_REG_WEN | AD717X_COMM_REG_WR |
		   AD717X_COMM_REG_RA(preg->addr);

	/* Fill the write buffer */
	regValue = preg->value;
	for(i = 0; i < preg->size; i++) {
		wrBuf[preg->size - i] = regValue & 0xFF;
		regValue >>= 8;
	}

	/* Compute the CRC */
	if(device->useCRC != AD717X_DISABLE) {
		crc8 = AD717X_ComputeCRC8(wrBuf, preg->size + 1);
		wrBuf[preg->size + 1] = crc8;
	}

	/* Write data to the device */
	ret = no_os_spi_write_and_read(device->spi_desc,
				       wrBuf,
				       (device->useCRC != AD717X_DISABLE) ?
				       preg->size + 2 : preg->size + 1);

	return ret;
}

/***************************************************************************//**
* @brief Resets the device.
*
* @param device - The handler of the instance of the driver.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD717X_Reset(ad717x_dev *device)
{
	int32_t ret = 0;
	uint8_t wrBuf[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	if(!device)
		return INVALID_VAL;

	ret = no_os_spi_write_and_read(device->spi_desc,
				       wrBuf,
				       8);

	return ret;
}

/***************************************************************************//**
* @brief Waits until a new conversion result is available.
*
* @param device  - The handler of the instance of the driver.
* @param timeout - Count representing the number of polls to be done until the
*                  function returns if no new data is available.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD717X_WaitForReady(ad717x_dev *device,
			    uint32_t timeout)
{
	ad717x_st_reg *statusReg;
	int32_t ret;
	int8_t ready = 0;

	if(!device || !device->regs)
		return INVALID_VAL;

	statusReg = AD717X_GetReg(device, AD717X_STATUS_REG);
	if (!statusReg)
		return INVALID_VAL;

	while(!ready && --timeout) {
		/* Read the value of the Status Register */
		ret = AD717X_ReadRegister(device, AD717X_STATUS_REG);
		if(ret < 0)
			return ret;

		/* Check the RDY bit in the Status Register */
		ready = (statusReg->value & AD717X_STATUS_REG_RDY) == 0;
	}

	return timeout ? 0 : TIMEOUT;
}

/***************************************************************************//**
* @brief Reads the conversion result from the device.
*
* @param device - The handler of the instance of the driver.
* @param pData  - Pointer to store the read data.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD717X_ReadData(ad717x_dev *device,
			int32_t* pData)
{
	ad717x_st_reg *dataReg;
	int32_t ret;

	if(!device || !device->regs)
		return INVALID_VAL;

	dataReg = AD717X_GetReg(device, AD717X_DATA_REG);
	if (!dataReg)
		return INVALID_VAL;

	/* Update the data register length with respect to device and options */
	ret = AD717X_ComputeDataregSize(device);

	/* Read the value of the Status Register */
	ret |= AD717X_ReadRegister(device, AD717X_DATA_REG);

	/* Get the read result */
	*pData = dataReg->value;

	return ret;
}

/***************************************************************************//**
* @brief Computes data register read size to account for bit number and status
* 		 read.
*
* @param device - The handler of the instance of the driver.
*
* @return 0in case of success or negative code in case of failure.
*******************************************************************************/
int32_t AD717X_ComputeDataregSize(ad717x_dev *device)
{
	ad717x_st_reg *reg_ptr;
	ad717x_st_reg *datareg_ptr;
	uint16_t case_var;

	/* Get interface mode register pointer */
	reg_ptr = AD717X_GetReg(device, AD717X_IFMODE_REG);
	/* Get data register pointer */
	datareg_ptr = AD717X_GetReg(device, AD717X_DATA_REG);
	case_var = reg_ptr->value & (AD717X_IFMODE_REG_DATA_STAT |
				     AD717X_IFMODE_REG_DATA_WL16);

	/* Compute data register size */
	datareg_ptr->size = 3;
	if ((case_var & AD717X_IFMODE_REG_DATA_WL16) == AD717X_IFMODE_REG_DATA_WL16)
		datareg_ptr->size--;
	if ((case_var & AD717X_IFMODE_REG_DATA_STAT) == AD717X_IFMODE_REG_DATA_STAT)
		datareg_ptr->size++;

	/* Get ID register pointer */
	reg_ptr = AD717X_GetReg(device, AD717X_ID_REG);

	/* If the part is 32/24 bit wide add a byte to the read */
	if((reg_ptr->value & AD717X_ID_REG_MASK) == AD7177_2_ID_REG_VALUE)
		datareg_ptr->size++;

	return 0;
}

/***************************************************************************//**
* @brief Computes the CRC checksum for a data buffer.
*
* @param pBuf    - Data buffer
* @param bufSize - Data buffer size in bytes
*
* @return Returns the computed CRC checksum.
*******************************************************************************/
uint8_t AD717X_ComputeCRC8(uint8_t * pBuf,
			   uint8_t bufSize)
{
	uint8_t i   = 0;
	uint8_t crc = 0;

	while(bufSize) {
		for(i = 0x80; i != 0; i >>= 1) {
			if(((crc & 0x80) != 0) != ((*pBuf & i) !=
						   0)) { /* MSB of CRC register XOR input Bit from Data */
				crc <<= 1;
				crc ^= AD717X_CRC8_POLYNOMIAL_REPRESENTATION;
			} else {
				crc <<= 1;
			}
		}
		pBuf++;
		bufSize--;
	}
	return crc;
}

/***************************************************************************//**
* @brief Computes the XOR checksum for a data buffer.
*
* @param pBuf    - Data buffer
* @param bufSize - Data buffer size in bytes
*
* @return Returns the computed XOR checksum.
*******************************************************************************/
uint8_t AD717X_ComputeXOR8(uint8_t * pBuf,
			   uint8_t bufSize)
{
	uint8_t xor = 0;

	while(bufSize) {
		xor ^= *pBuf;
		pBuf++;
		bufSize--;
	}
	return xor;
}

/***************************************************************************//**
* @brief Updates the CRC settings.
*
* @param device - The handler of the instance of the driver.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD717X_UpdateCRCSetting(ad717x_dev *device)
{
	ad717x_st_reg *interfaceReg;

	if(!device || !device->regs)
		return INVALID_VAL;

	interfaceReg = AD717X_GetReg(device, AD717X_IFMODE_REG);
	if (!interfaceReg)
		return INVALID_VAL;

	/* Get CRC State. */
	if(AD717X_IFMODE_REG_CRC_STAT(interfaceReg->value)) {
		device->useCRC = AD717X_USE_CRC;
	} else if(AD717X_IFMODE_REG_XOR_STAT(interfaceReg->value)) {
		device->useCRC = AD717X_USE_XOR;
	} else {
		device->useCRC = AD717X_DISABLE;
	}

	return 0;
}

/**
 * @brief Configure ODR for the device
 * @param dev - The AD717x Device descriptor
 * @param filtcon_id - Filter Configuration Register ID (Number)
 * @param odr_sel - ODR[4:0] bitfield value as a decimal
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad717x_configure_device_odr(ad717x_dev *dev,
				    uint8_t filtcon_id,
				    uint8_t odr_sel)
{
	ad717x_st_reg *filtcon_reg;
	int32_t ret;

	/* Retrieve the FILTCON register */
	filtcon_reg = AD717X_GetReg(dev,
				    AD717X_FILTCON0_REG + filtcon_id);
	if (!filtcon_reg) {
		return -EINVAL;
	}

	/* Clear the ODR bits, configure the requested ODR */
	filtcon_reg->value &= ~(AD717x_ODR_MSK);
	filtcon_reg->value |= AD717X_FILT_CONF_REG_ODR(odr_sel);

	ret = AD717X_WriteRegister(dev, AD717X_FILTCON0_REG + filtcon_id);
	if (ret) {
		return ret;
	}

	return 0;
}

/***************************************************************************//**
* @brief Initializes the AD717X.
*
* @param device     - The device structure.
* @param init_param - The structure that contains the device initial
* 		       parameters.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD717X_Init(ad717x_dev **device,
		    ad717x_init_param init_param)
{
	ad717x_dev *dev;
	int32_t ret;
	ad717x_st_reg *preg;
	uint8_t setup_index;
	uint8_t ch_index;

	dev = (ad717x_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->regs = init_param.regs;
	dev->num_regs = init_param.num_regs;

	/* Initialize the SPI communication. */
	ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
	if (ret < 0)
		return ret;

	/*  Reset the device interface.*/
	ret = AD717X_Reset(dev);
	if (ret < 0)
		return ret;

	/* Initialize ADC mode register. */
	ret = AD717X_WriteRegister(dev, AD717X_ADCMODE_REG);
	if(ret < 0)
		return ret;

	/* Initialize Interface mode register. */
	ret = AD717X_WriteRegister(dev, AD717X_IFMODE_REG);
	if(ret < 0)
		return ret;

	/* Get CRC State */
	ret = AD717X_UpdateCRCSetting(dev);
	if(ret < 0)
		return ret;

	/* Initialize registers AD717X_GPIOCON_REG through AD717X_OFFSET0_REG */
	preg = AD717X_GetReg(dev, AD717X_GPIOCON_REG);
	if (!preg)
		return INVALID_VAL;

	while (preg && preg->addr != AD717X_OFFSET0_REG) {
		if (preg->addr == AD717X_ID_REG) {
			preg ++;
			continue;
		}

		ret = AD717X_WriteRegister(dev, preg->addr);
		if (ret < 0)
			break;
		preg ++;
	}

	/* Read ID register to identify the part */
	ret = AD717X_ReadRegister(dev, AD717X_ID_REG);
	if(ret < 0)
		return ret;
	dev->active_device = init_param.active_device;
	dev->num_channels = init_param.num_channels;

	for (setup_index = 0; setup_index < init_param.num_setups; setup_index++) {
		/* Set Polarity */
		ret = ad717x_set_polarity(dev, init_param.setups[setup_index].bi_unipolar,
					  setup_index);
		if (ret < 0)
			return ret;

		/* Select the reference source */
		ret = ad717x_set_reference_source(dev,
						  init_param.setups[setup_index].ref_source, setup_index);
		if (ret < 0)
			return ret;

		/* Enable reference and input buffers */
		ret = ad717x_enable_input_buffer(dev,
						 init_param.setups[setup_index].input_buff,
						 init_param.setups[setup_index].ref_buff,
						 setup_index);
		if (ret < 0)
			return ret;

		ret = ad717x_configure_device_odr(dev, setup_index,
						  init_param.filter_configuration[setup_index].odr);
		if (ret < 0)
			return ret;
	}

	/* Set Conversion Mode */
	ret = ad717x_set_adc_mode(dev, init_param.mode);
	if (ret < 0)
		return ret;

	/*  Connect Analog Inputs, Assign Setup, Disable all channels */
	for (ch_index = 0; ch_index < init_param.num_channels; ch_index++) {
		ret = ad717x_connect_analog_input(dev, ch_index,
						  init_param.chan_map[ch_index].analog_inputs);
		if (ret < 0)
			return ret;

		ret = ad717x_assign_setup(dev, ch_index,
					  init_param.chan_map[ch_index].setup_sel);
		if (ret < 0)
			return ret;

		ret = ad717x_set_channel_status(dev,ch_index,
						init_param.chan_map[ch_index].channel_enable);
		if (ret < 0)
			return ret;
	}
	*device = dev;

	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by AD717X_Init().
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t AD717X_remove(ad717x_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	no_os_free(dev);

	return ret;
}
