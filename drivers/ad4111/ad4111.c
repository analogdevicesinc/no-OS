/***************************************************************************//**
 *   @file   ad4111.c
 *   @brief  Implementation of AD4111 driver.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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
#include "ad4111.h"

/* Error codes */
#define INVALID_VAL -1 /* Invalid argument */
#define COMM_ERR    -2 /* Communication error on receive */
#define TIMEOUT     -3 /* A timeout has occured */

/***************************************************************************//**
* @brief  Searches through the list of registers of the driver instance and
*         retrieves a pointer to the register that matches the given address.
*
* @param device - The handler of the instance of the driver.
* @param reg_address - The address to be used to find the register.
*
* @return A pointer to the register if found or 0.
*******************************************************************************/
struct ad4111_st_reg *ad4111_get_reg(struct ad4111_dev *device,
		uint8_t reg_address)
{
        uint8_t i;
        struct ad4111_st_reg *reg = 0;

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
* @addr - The address of the register to be read. The value will be stored
*         inside the register structure that holds info about this register.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t ad4111_read_register(struct ad4111_dev *device,
                            uint8_t addr)
{
        int32_t ret       = 0;
        uint8_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        uint8_t i         = 0;
        uint8_t check8    = 0;
        uint8_t msg_buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        struct ad4111_st_reg *p_reg;

        if(!device)
                return INVALID_VAL;

        p_reg = ad4111_get_reg(device, addr);
        if (!p_reg)
                return INVALID_VAL;

        /* Build the Command word */
        buffer[0] = AD4111_COMM_REG_WEN | AD4111_COMM_REG_RD |
        		AD4111_COMM_REG_RA(p_reg->addr);

        /* Read data from the device */
        ret = spi_write_and_read(device->spi_desc,
                                 buffer,
                                 ((device->useCRC != AD4111_DISABLE) ?
                                		 p_reg->size + 1 :
										 p_reg->size) + 1);
        if(ret < 0)
                return ret;

        /* Check the CRC */
        if(device->useCRC == AD4111_USE_CRC) {
        	msg_buf[0] = AD4111_COMM_REG_WEN | AD4111_COMM_REG_RD |
                		AD4111_COMM_REG_RA(p_reg->addr);
                for(i = 1; i < p_reg->size + 2; ++i) {
                	msg_buf[i] = buffer[i];
                }
                check8 = ad4111_compute_crc8(msg_buf, p_reg->size + 2);
        }
        if(device->useCRC == AD4111_USE_XOR) {
        	msg_buf[0] = AD4111_COMM_REG_WEN | AD4111_COMM_REG_RD |
                		AD4111_COMM_REG_RA(p_reg->addr);
                for(i = 1; i < p_reg->size + 2; ++i) {
                	msg_buf[i] = buffer[i];
                }
                check8 = ad4111_compute_xor8(msg_buf, p_reg->size + 2);
        }

        if(check8 != 0) {
                /* ReadRegister checksum failed. */
                return COMM_ERR;
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
* @brief Writes the value of the specified register.
*
* @param device - The handler of the instance of the driver.
* @param addr   - The address of the register to be written with the value
* 				  stored inside the register structure that holds info about
* 				  this register.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t ad4111_write_register(struct ad4111_dev *device,
                             uint8_t addr)
{
        int32_t ret      = 0;
        int32_t reg_value = 0;
        uint8_t wr_buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        uint8_t i        = 0;
        uint8_t crc8     = 0;
        struct ad4111_st_reg *preg;

        if(!device)
                return INVALID_VAL;

        preg = ad4111_get_reg(device, addr);
        if (!preg)
                return INVALID_VAL;

        /* Build the Command word */
        wr_buf[0] = AD4111_COMM_REG_WEN | AD4111_COMM_REG_WR |
        		AD4111_COMM_REG_RA(preg->addr);

        /* Fill the write buffer */
        reg_value = preg->value;
        for(i = 0; i < preg->size; i++) {
        	wr_buf[preg->size - i] = reg_value & 0xFF;
        	reg_value >>= 8;
        }

        /* Compute the CRC */
        if(device->useCRC != AD4111_DISABLE) {
                crc8 = ad4111_compute_crc8(wr_buf, preg->size + 1);
                wr_buf[preg->size + 1] = crc8;
        }

        /* Write data to the device */
        ret = spi_write_and_read(device->spi_desc,
        		wr_buf,
                                 (device->useCRC != AD4111_DISABLE) ?
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
int32_t ad4111_reset(struct ad4111_dev *device)
{
        int32_t ret = 0;
        uint8_t wr_buf[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

        if(!device)
                return INVALID_VAL;

        ret = spi_write_and_read(device->spi_desc,
        		wr_buf,
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
int32_t ad4111_wait_for_ready(struct ad4111_dev *device,
                            uint32_t timeout)
{
        struct ad4111_st_reg *status_reg;
        int32_t ret;
        int8_t ready = 0;

        if(!device || !device->regs)
                return INVALID_VAL;

        status_reg = ad4111_get_reg(device, AD4111_STATUS_REG);
        if (!status_reg)
                return INVALID_VAL;

        while(!ready && --timeout) {
                /* Read the value of the Status Register */
                ret = ad4111_read_register(device, AD4111_STATUS_REG);
                if(ret < 0)
                        return ret;

                /* Check the RDY bit in the Status Register */
                ready = (status_reg->value & AD4111_STATUS_REG_RDY) == 0;
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
int32_t ad4111_read_data(struct ad4111_dev *device,
                        int32_t* p_data)
{
        struct ad4111_st_reg *data_reg;
        int32_t ret = 0;

        if(!device || !device->regs)
                return INVALID_VAL;

        data_reg = ad4111_get_reg(device, AD4111_DATA_REG);
        if (!data_reg)
                return INVALID_VAL;

        ret |= ad4111_compute_datareg_size(device);

        /* Read the value of the Status Register */
        ret |= ad4111_read_register(device, AD4111_DATA_REG);

        /* Get the read result */
        *p_data = data_reg->value;

        return ret;
}

/***************************************************************************//**
* @brief Computes the CRC checksum for a data buffer.
*
* @param pBuf    - Data buffer
* @param bufSize - Data buffer size in bytes
*
* @return Returns the computed CRC checksum.
*******************************************************************************/
uint8_t ad4111_compute_crc8(uint8_t * p_buf,
		uint8_t buf_size)
{
        uint8_t i   = 0;
        uint8_t crc = 0;

        while(buf_size) {
                for(i = 0x80; i != 0; i >>= 1) {
					/* MSB of CRC register XOR input Bit from Data */
                        if(((crc & 0x80) != 0) != ((*p_buf & i) != 0)) {
                                crc <<= 1;
                                crc ^= AD4111_CRC8_POLYNOMIAL_REPRESENTATION;
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
* @brief Computes the XOR checksum for a data buffer.
*
* @param pBuf    - Data buffer
* @param bufSize - Data buffer size in bytes
*
* @return Returns the computed XOR checksum.
*******************************************************************************/
uint8_t ad4111_compute_xor8(uint8_t * p_buf,
                           uint8_t buf_size)
{
        uint8_t xor = 0;

        while(buf_size) {
                xor ^= *p_buf;
                p_buf++;
                buf_size--;
        }
        return xor;
}

/***************************************************************************//**
* @brief Computes data register read size to account for bit number and status
* 		 read.
*
* @param device - The handler of the instance of the driver.
*
* @return 0in case of success or negative code in case of failure.
*******************************************************************************/
int32_t ad4111_compute_datareg_size(struct ad4111_dev *device)
{
	int32_t ret = 0;
	struct ad4111_st_reg *ifreg_ptr;
	struct ad4111_st_reg *datareg_ptr;
	uint16_t case_var;

	/* Get interface mode register pointer */
	ifreg_ptr = ad4111_get_reg(device, AD4111_IFMODE_REG);
	/* Get data register pointer */
	datareg_ptr = ad4111_get_reg(device, AD4111_DATA_REG);

	case_var = ifreg_ptr->value & (AD4111_IFMODE_REG_DATA_STAT |
			AD4111_IFMODE_REG_DATA_WL16);

	/* Compute data register size */
    switch(case_var) {
    case 0:
    	datareg_ptr->size = 3;
    	break;
    case AD4111_IFMODE_REG_DATA_WL16:
    	datareg_ptr->size = 2;
    	break;
    case AD4111_IFMODE_REG_DATA_STAT:
    	datareg_ptr->size = 4;
    	break;
    case (AD4111_IFMODE_REG_DATA_STAT | AD4111_IFMODE_REG_DATA_WL16):
    	datareg_ptr->size = 3;
    	break;
    default:
    	datareg_ptr->size = 3;
    	break;
    }

	return ret;
}

/***************************************************************************//**
* @brief Updates the CRC settings.
*
* @param device - The handler of the instance of the driver.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t ad4111_update_crc_setting(struct ad4111_dev *device)
{
        struct ad4111_st_reg *interface_reg;

        if(!device || !device->regs)
                return INVALID_VAL;

        interface_reg = ad4111_get_reg(device, AD4111_IFMODE_REG);
        if (!interface_reg)
                return INVALID_VAL;

        /* Get CRC State. */
        if(AD4111_IFMODE_REG_CRC_STAT(interface_reg->value)) {
                device->useCRC = AD4111_USE_CRC;
        } else if(AD4111_IFMODE_REG_XOR_STAT(interface_reg->value)) {
                device->useCRC = AD4111_USE_XOR;
        } else {
                device->useCRC = AD4111_DISABLE;
        }

        return 0;
}

/***************************************************************************//**
* @brief Initializes the AD4111.
*
* @param device     - The device structure.
* @param init_param - The structure that contains the device initial parameters.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t ad4111_init(struct ad4111_dev **device,
		    struct ad4111_init_param init_param)
{
        struct ad4111_dev *dev;
        int32_t ret;
        struct ad4111_st_reg *preg;

        dev = (struct ad4111_dev *)malloc(sizeof(*dev));
        if (!dev)
                return -1;

        dev->regs = init_param.regs;
        dev->num_regs = init_param.num_regs;

        /* Initialize the SPI communication. */
        ret = spi_init(&dev->spi_desc, init_param.spi_init);
        if (ret < 0)
                return ret;

        /*  Reset the device interface.*/
        ret = ad4111_reset(dev);
        if (ret < 0)
                return ret;

        mdelay(1000);

        /* Initialize ADC mode register. */
        ret = ad4111_write_register(dev, AD4111_ADCMODE_REG);
        if(ret < 0)
                return ret;

        /* Initialize Interface mode register. */
        ret = ad4111_write_register(dev, AD4111_IFMODE_REG);
        if(ret < 0)
                return ret;

        /* Get CRC State */
        ret = ad4111_update_crc_setting(dev);
        if(ret < 0)
                return ret;

        /* Initialize registers AD4111_GPIOCON_REG through AD4111_OFFSET0_REG */
        preg = ad4111_get_reg(dev, AD4111_GPIOCON_REG);
        if (!preg)
                return INVALID_VAL;

        while (preg && preg->addr != AD4111_OFFSET0_REG) {
                if (preg->addr == AD4111_ID_REG) {
                        preg ++;
                        continue;
                }

                ret = ad4111_write_register(dev, preg->addr);
                if (ret < 0)
                        break;
                preg ++;
        }

        *device = dev;

        return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad4111_init().
 *
 * @param dev - The device structure.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad4111_remove(struct ad4111_dev *dev)
{
        int32_t ret;

        ret = spi_remove(dev->spi_desc);

        free(dev);

        return ret;
}

