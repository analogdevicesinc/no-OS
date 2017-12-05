/******************************************************************************
*   @file   AD525x.c
*   @brief  Implementation of AD525X Driver for Microblaze processor.
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
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
*
******************************************************************************/

/*****************************************************************************/
/****************************** Include Files ********************************/
/*****************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include "platform_drivers.h"
#include "ad525x.h"

#define MSB_BYTE_MASK       0xFF00
#define LSB_BYTE_MASK       0x00FF
#define ONEBYTE_OFFSET      0x8

/*****************************************************************************/
/***************************** Constant definition ***************************/
/*****************************************************************************/
static const ad525x_chip_info CHIP_INFO[] = {
    [ID_AD5232] = {
        .num_channels = 2,
        .comm_type = SPI,
        .num_position = 256,
    },
    [ID_AD5235] = {
        .num_channels = 2,
        .comm_type = SPI,
        .num_position = 1024,
    },
    [ID_AD5252] = {
        .num_channels = 2,
        .comm_type = I2C,
        .num_position = 256,
    },
    [ID_AD5251] = {
        .num_channels = 2,
        .comm_type = I2C,
        .num_position = 64,
    },
    [ID_AD5254] = {
        .num_channels = 4,
        .comm_type = I2C,
        .num_position = 256,
    },
    [ID_AD5253] = {
        .num_channels = 4,
        .comm_type = I2C,
        .num_position = 64,
    },
    [ID_ADN2850] = {
        .num_channels = 2,
        .comm_type = SPI,
        .num_position = 1024,
    }
};

/*****************************************************************************/
/*************************** Functions definitions ***************************/
/*****************************************************************************/

/**************************************************************************//**
 * @brief Initialize SPI and Initial Values for AD5172 Board.
 *
 * @param + device     - The device structure.
 *        + init_param - The structure that contains the device initial
 * 		         parameters.
 *
 * @return success
******************************************************************************/
char AD525X_Init(ad525x_dev **device,
		 ad525x_init_param init_param)
{
	ad525x_dev *dev;
    char status;

	dev = (ad525x_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

    dev->this_device = init_param.this_device;

    if(CHIP_INFO[dev->this_device].comm_type == SPI)
    {
        /* CPHA = 0; CPOL = 0; */
	    status = spi_init(&dev->spi_desc, init_param.spi_init);
    }
    else
    {
	    status = i2c_init(&dev->i2c_desc, init_param.i2c_init);
    }

	status |= gpio_get(&dev->gpio_reset, init_param.gpio_reset);
	status |= gpio_get(&dev->gpio_shutdown, init_param.gpio_shutdown);
	status |= gpio_get(&dev->gpio_ready, init_param.gpio_ready);
	status |= gpio_get(&dev->gpio_wpbf, init_param.gpio_wpbf);

    /* Deactivate Hardware Reset */
    AD525X_RESET_OUT;
    AD525X_RESET_HIGH;
    /* Deactivate Shutdown Pin  */
    AD525X_SHUTDOWN_OUT;
    AD525X_SHUTDOWN_HIGH;
    /* Deactivate Write Protect Pin */
    AD525X_WP_BF_OUT;
    AD525X_WP_BF_HIGH;
    /* Setup the RDY input */
    AD525X_READY_IN;

	*device = dev;

    return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by AD525X_Init().
 *
 * @param + dev - The device structure.
 *
 * @return ret - The result of the remove procedure.
*******************************************************************************/
int32_t ad525x_remove(ad525x_dev *dev)
{
	int32_t ret;

	if (CHIP_INFO[dev->this_device].comm_type == SPI)
		ret = spi_remove(dev->spi_desc);
	else
		ret = i2c_remove(dev->i2c_desc);

	if (dev->gpio_shutdown)
		ret |= gpio_remove(dev->gpio_shutdown);

	if (dev->gpio_reset)
		ret |= gpio_remove(dev->gpio_reset);

	if (dev->gpio_ready)
		ret |= gpio_remove(dev->gpio_ready);

	if (dev->gpio_wpbf)
		ret |= gpio_remove(dev->gpio_wpbf);

	free(dev);

	return ret;
}

/**************************************************************************//**
 * @brief Read data from the EEMEM
 *
 * @param + dev     - The device structure.
 *        + address - desired address of the EEMEM memory
 *
 *
 * @return success
******************************************************************************/
unsigned short AD525X_ReadMem(ad525x_dev *dev,
                              unsigned char address)
{
    unsigned char dataBuffer[3] = {0,};
    unsigned short data = 0;

    if(CHIP_INFO[dev->this_device].comm_type == SPI)
    {
        /* Sending the command, reading the result on the next frame */
        dataBuffer[0] |= AD525X_CMD_SPI_MEM2SREG << AD525X_CMD_SPI_OFFSET;
        dataBuffer[0] |= address & AD525X_MEM_ADDR_MASK;
        /* 3 byte data word */
        if((dev->this_device == ID_AD5235) || (dev->this_device == ID_ADN2850))
        {
		spi_write_and_read(dev->spi_desc,
				   dataBuffer,
				   3);
            dataBuffer[0] &= AD525X_CMD_NOP << AD525X_CMD_SPI_OFFSET;

		spi_write_and_read(dev->spi_desc,
				   dataBuffer,
				   3);
            data = (dataBuffer[1] << ONEBYTE_OFFSET) | dataBuffer[2];
            return data;
        }
        else /* 2 byte data word */
        {
		spi_write_and_read(dev->spi_desc,
				   dataBuffer,
				   2);
            return (unsigned short)dataBuffer[1];
        }
    }
    else /* Communication interface is I2C */
    {
        dataBuffer[0] &= ~AD525X_I2C_CMD_OR_REG;             // reset CMD/REG_n
        dataBuffer[0] |= AD525X_I2C_EE_OR_RDAC;              // set EE/RDAC_n
        dataBuffer[0] |= address & AD525X_I2C_MEM_ADDR_MASK; // set address
        /* Dummy write to select the desired register */
	    i2c_write(dev->i2c_desc,
		      dataBuffer,
		      1,
		      1);
        dataBuffer[0] &= AD525X_CMD_NOP;
	    i2c_read(dev->i2c_desc,
		 dataBuffer,
		 1,
		 1);
        data = (unsigned short)dataBuffer[0];
        return data;
    }
}

/*****SPI_*********************************************************************//**
 * @brief Write data to EEMEM.
 *
 * @param + dev     - The device structure.
 *        + address - desired address of the EEMEM memory
 *        + data    - the data which will be written to the memory
 *
 * @return success
******************************************************************************/
void AD525X_WriteMem(ad525x_dev *dev,
                     unsigned char address,
                     unsigned short data)
{
    unsigned char dataBuffer[3] = {0,};

    if(CHIP_INFO[dev->this_device].comm_type == SPI)
    {
        /* Sending the command, reading the result on the next frame */
        dataBuffer[0] |= AD525X_CMD_SPI_SREG2MEM << AD525X_CMD_SPI_OFFSET;
        dataBuffer[0] |= address & AD525X_MEM_ADDR_MASK;
        if((dev->this_device == ID_AD5235) || (dev->this_device == ID_ADN2850)) /* 3 byte data word */
        {
            dataBuffer[1] = (data & MSB_BYTE_MASK) >> ONEBYTE_OFFSET;
            dataBuffer[2] = data & LSB_BYTE_MASK;
		spi_write_and_read(dev->spi_desc,
				   dataBuffer,
				   3);
        }
        else /* 2 byte data word */
        {
            dataBuffer[1] = data & LSB_BYTE_MASK;
		spi_write_and_read(dev->spi_desc,
				   dataBuffer,
				   2);
        }
    }
    else /* Communication interface is I2C */
    {
        dataBuffer[0] &= ~AD525X_I2C_CMD_OR_REG;         // reset CMD/REG_n
        dataBuffer[0] |= AD525X_I2C_EE_OR_RDAC;          // set EE/RDAC_n
        dataBuffer[0] |= address & AD525X_MEM_ADDR_MASK; // set address

        dataBuffer[1] = (data & LSB_BYTE_MASK);
	    i2c_write(dev->i2c_desc,
		      dataBuffer,
		      2,
		      1);
    }
}

/**************************************************************************//**
 * @brief Read data from the RDAC register
 *
 * @param + dev     - The device structure.
 *        + address - desired address of the RDAC registers
 *
 *
 * @return success
******************************************************************************/
unsigned short AD525X_ReadRdac(ad525x_dev *dev,
                               unsigned char address)
{
    unsigned char dataBuffer[3] = {0,};
    unsigned short data = 0;

    if(CHIP_INFO[dev->this_device].comm_type == SPI)
    {
        /* Sending the command, reading the result on the next frame */
        dataBuffer[0] |= AD525X_CMD_SPI_RDAC2SREG << AD525X_CMD_SPI_OFFSET;
        dataBuffer[0] |= address & AD525X_MEM_ADDR_MASK;
        /* 3 byte data word */
        if((dev->this_device == ID_AD5235) || (dev->this_device == ID_ADN2850))
        {
		spi_write_and_read(dev->spi_desc,
				   dataBuffer,
				   3);
            mdelay(50);
            dataBuffer[0] &= AD525X_CMD_NOP << AD525X_CMD_SPI_OFFSET;
		spi_write_and_read(dev->spi_desc,
				   dataBuffer,
				   3);
            data = (dataBuffer[1] << ONEBYTE_OFFSET) | dataBuffer[2];
            return (data & AD525X_DATA10_MASK);
        }
        else /* 2 byte data word */
        {
		spi_write_and_read(dev->spi_desc,
				   dataBuffer,
				   2);
            mdelay(50);
            /* Sending a dummy frame to read the result */
            dataBuffer[0] &= AD525X_CMD_NOP << AD525X_CMD_SPI_OFFSET;
		spi_write_and_read(dev->spi_desc,
				   dataBuffer,
				   2);
            data = (unsigned short)dataBuffer[1];
            return data;
        }
    }
    else /* Communication interface is I2C */
    {
        dataBuffer[0] &= ~AD525X_I2C_CMD_OR_REG;         // reset CMD/REG_n
        dataBuffer[0] &= ~AD525X_I2C_EE_OR_RDAC;         // reset EE/RDAC_n
        dataBuffer[0] |= address & AD525X_MEM_ADDR_MASK; // set address
        /* Dummy write to select the desired register */
	    i2c_write(dev->i2c_desc,
		      dataBuffer,
		      1,
		      1);
        dataBuffer[0] &= AD525X_CMD_NOP;
	    i2c_read(dev->i2c_desc,
		 dataBuffer,
		 1,
		 1);
        data = (unsigned short)dataBuffer[0];
        return data;
    }
}

/**************************************************************************//**
 * @brief Write data to RDAC register.
 *
 * @param + dev     - The device structure.
 *        + address - desired address of the RDAC register
 *        + data    - the data which will be written to the RDAC register
 *
 * @return success
******************************************************************************/
void AD525X_WriteRdac(ad525x_dev *dev,
                      unsigned char address,
                      unsigned short data)
{
    unsigned char dataBuffer[3] = {0,};

    if(CHIP_INFO[dev->this_device].comm_type == SPI)
    {
        /* Sending the command, reading the result on the next frame */
        dataBuffer[0] |= AD525X_CMD_SPI_SREG2RDAC << AD525X_CMD_SPI_OFFSET;
        dataBuffer[0] |= address & AD525X_MEM_ADDR_MASK;
        if((dev->this_device == ID_AD5235) || (dev->this_device == ID_ADN2850)) /* 3 byte data word */
        {
            dataBuffer[1] = (data & MSB_BYTE_MASK) >> ONEBYTE_OFFSET;
            dataBuffer[2] = data & LSB_BYTE_MASK;
		spi_write_and_read(dev->spi_desc,
				   dataBuffer,
				   3);
        }
        else /* 2 byte data word */
        {
            dataBuffer[1] = data & LSB_BYTE_MASK;
		spi_write_and_read(dev->spi_desc,
				   dataBuffer,
				   2);
        }
    }
    else /* Communication interface is I2C */
    {
        dataBuffer[0] &= ~AD525X_I2C_CMD_OR_REG;         // reset CMD/REG_n
        dataBuffer[0] &= ~AD525X_I2C_EE_OR_RDAC;         // reset EE/RDAC_n
        dataBuffer[0] |= address & AD525X_MEM_ADDR_MASK; // set address

        dataBuffer[1] = (data & LSB_BYTE_MASK);
	    i2c_write(dev->i2c_desc,
		      dataBuffer,
		      2,
		      1);
    }
    mdelay(25);
}

/**************************************************************************//**
 * @brief Write quick commands to the device.
 *
 * @param + dev     - The device structure.
 *        + command - desired command, the functionality of a command it may
 *                    vary at diffrent devices
 *
 * @return success
******************************************************************************/
void AD525X_WriteCommand(ad525x_dev *dev,
                         unsigned char command,
                         unsigned char address)
{
    unsigned char dataBuffer[3] = {0,};

    if(CHIP_INFO[dev->this_device].comm_type == SPI)
    {
        /* Sending the command, reading the result on the next frame */
        command &= AD525X_CMD_MASK;
        /* Command adjustment (because of the diffrence between command
        representations) */
        if((command >= AD525X_CMD_DECRDAC_6DB) && (command <= AD525X_CMD_RESET))
        {
            command += 1;
        }
        else if(command >= AD525X_CMD_INCRDAC_6DB)
        {
            command += 4;
        }
        dataBuffer[0] |= command << AD525X_CMD_SPI_OFFSET;
        if(command == AD525X_CMD_RESET)
        {
            dataBuffer[0] &= 0xF0;
        }
        else
        {
            dataBuffer[0] |= (address & AD525X_RDAC_ADDR_MASK_1BIT);
        }

        if((dev->this_device == ID_AD5235) || (dev->this_device == ID_ADN2850)) /* 3 byte data word */
        {
		spi_write_and_read(dev->spi_desc,
				   dataBuffer,
				   3);
            if(command == AD525X_CMD_MEM2RDAC)
            {
                dataBuffer[0] &= AD525X_CMD_NOP;
		    spi_write_and_read(dev->spi_desc,
				   dataBuffer,
				   3);
            }
        }
        else /* 2 byte data word */
        {
		spi_write_and_read(dev->spi_desc,
				   dataBuffer,
				   2);
            if(command == AD525X_CMD_MEM2RDAC)
            {
		    spi_write_and_read(dev->spi_desc,
				   dataBuffer,
				   2);
            }
        }
    }
    else /* Communication interface is I2C */
    {
        dataBuffer[0] |= AD525X_I2C_CMD_OR_REG;                // set CMD/REG_n
        dataBuffer[0] |= (command & AD525X_CMD_MASK) << AD525X_CMD_I2C_OFFSET;
        dataBuffer[0] |= address & AD525X_RDAC_ADDR_MASK_3BIT; // set address

	    i2c_write(dev->i2c_desc,
		      dataBuffer,
		      2,
		      1);
        if(command == AD525X_CMD_MEM2RDAC)
        {
            dataBuffer[0] &= AD525X_CMD_NOP;
		i2c_write(dev->i2c_desc,
		      dataBuffer,
		      1,
		      1);
        }
    }
    mdelay(25);
}
