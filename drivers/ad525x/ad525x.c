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
#include "ad525x.h"
#include "Communication.h"
#include "TIME.h"

#define MSB_BYTE_MASK       0xFF00
#define LSB_BYTE_MASK       0x00FF
#define ONEBYTE_OFFSET      0x8
/*****************************************************************************/
/**************************** Private variables ******************************/
/*****************************************************************************/
struct ad525x_chip_info {
    unsigned char num_channels;
    CommType_t comm_type;
    unsigned short num_position;
};

static const struct ad525x_chip_info AD525X_CHIP_INFO[] = {
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

/* Actual device type */
AD525X_type_t this_device;

/*****************************************************************************/
/*************************** Functions definitions ***************************/
/*****************************************************************************/

/**************************************************************************//**
 * @brief Initialize SPI and Initial Values for AD5172 Board.
 *
 * @param The current device type.
 *
 * @return success
******************************************************************************/
char AD525X_Init(AD525X_type_t device)
{
    char status = -1;

    this_device = device;

    if(AD525X_CHIP_INFO[this_device].comm_type == SPI)
    {
        /* CPHA = 0; CPOL = 0; */
        status = SPI_Init(0, 100000, 0, 0);
    }
    else
    {
        status = I2C_Init(100000);
    }

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

    return status;
}

/**************************************************************************//**
 * @brief Read data from the EEMEM
 *
 * @param + slaveId - slave device ID in case of SPI interface, and
 *                    programmable device address in case of the I2C
 *        + address - desired address of the EEMEM memory
 *
 *
 * @return success
******************************************************************************/
unsigned short AD525X_ReadMem(unsigned char slaveId,
                              unsigned char address)
{
    unsigned char dataBuffer[3] = {0,};
    unsigned char i2cSlaveAddr = 0;
    unsigned short data = 0;

    if(AD525X_CHIP_INFO[this_device].comm_type == SPI)
    {
        /* Sending the command, reading the result on the next frame */
        dataBuffer[0] |= AD525X_CMD_SPI_MEM2SREG << AD525X_CMD_SPI_OFFSET;
        dataBuffer[0] |= address & AD525X_MEM_ADDR_MASK;
        /* 3 byte data word */
        if((this_device == ID_AD5235) || (this_device == ID_ADN2850))
        {
            SPI_Write(slaveId,
                      dataBuffer,
                      3);
            dataBuffer[0] &= AD525X_CMD_NOP << AD525X_CMD_SPI_OFFSET;
            SPI_Read(slaveId,
                            dataBuffer,
                            3);
            data = (dataBuffer[1] << ONEBYTE_OFFSET) | dataBuffer[2];
            return data;
        }
        else /* 2 byte data word */
        {
            SPI_Read(slaveId,
                     dataBuffer,
                     2);
            return (unsigned short)dataBuffer[1];
        }
    }
    else /* Communication interface is I2C */
    {
        /* Setup I2C address */
        i2cSlaveAddr |= (AD525X_I2C_HARD_ADDR | \
                        (slaveId & AD525X_I2C_PIN_ADDR_MASK));
        dataBuffer[0] &= ~AD525X_I2C_CMD_OR_REG;             // reset CMD/REG_n
        dataBuffer[0] |= AD525X_I2C_EE_OR_RDAC;              // set EE/RDAC_n
        dataBuffer[0] |= address & AD525X_I2C_MEM_ADDR_MASK; // set address
        /* Dummy write to select the desired register */
        I2C_Write(i2cSlaveAddr,
                  dataBuffer,
                  1,
                  1);
        dataBuffer[0] &= AD525X_CMD_NOP;
        I2C_Read(i2cSlaveAddr,
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
 * @param + slaveId - slave device ID in case of SPI interface, and
 *                    programmable device address in case of the I2C
 *        + address - desired address of the EEMEM memory
 *        + data - the data which will be written to the memory
 *
 * @return success
******************************************************************************/
void AD525X_WriteMem(unsigned char slaveId,
                     unsigned char address,
                     unsigned short data)
{
    unsigned char dataBuffer[3] = {0,};
    unsigned char i2cSlaveAddr = 0;

    if(AD525X_CHIP_INFO[this_device].comm_type == SPI)
    {
        /* Sending the command, reading the result on the next frame */
        dataBuffer[0] |= AD525X_CMD_SPI_SREG2MEM << AD525X_CMD_SPI_OFFSET;
        dataBuffer[0] |= address & AD525X_MEM_ADDR_MASK;
        if((this_device == ID_AD5235) || (this_device == ID_ADN2850)) /* 3 byte data word */
        {
            dataBuffer[1] = (data & MSB_BYTE_MASK) >> ONEBYTE_OFFSET;
            dataBuffer[2] = data & LSB_BYTE_MASK;
            SPI_Write(slaveId,
                      dataBuffer,
                      3);
        }
        else /* 2 byte data word */
        {
            dataBuffer[1] = data & LSB_BYTE_MASK;
            SPI_Write(slaveId,
                      dataBuffer,
                      2);
        }
    }
    else /* Communication interface is I2C */
    {
        /* Setup I2C address */
        i2cSlaveAddr |= (AD525X_I2C_HARD_ADDR | \
                        (slaveId & AD525X_I2C_PIN_ADDR_MASK));
        dataBuffer[0] &= ~AD525X_I2C_CMD_OR_REG;         // reset CMD/REG_n
        dataBuffer[0] |= AD525X_I2C_EE_OR_RDAC;          // set EE/RDAC_n
        dataBuffer[0] |= address & AD525X_MEM_ADDR_MASK; // set address

        dataBuffer[1] = (data & LSB_BYTE_MASK);
        I2C_Write(i2cSlaveAddr,
                  dataBuffer,
                  2,
                  1);
    }
}

/**************************************************************************//**
 * @brief Read data from the RDAC register
 *
 * @param + slaveId - slave device ID in case of SPI interface, and
 *                    programmable device address in case of the I2C
 *        + address - desired address of the RDAC registers
 *
 *
 * @return success
******************************************************************************/
unsigned short AD525X_ReadRdac(unsigned char slaveId,
                               unsigned char address)
{
    unsigned char dataBuffer[3] = {0,};
    unsigned char i2cSlaveAddr = 0;
    unsigned short data = 0;

    if(AD525X_CHIP_INFO[this_device].comm_type == SPI)
    {
        /* Sending the command, reading the result on the next frame */
        dataBuffer[0] |= AD525X_CMD_SPI_RDAC2SREG << AD525X_CMD_SPI_OFFSET;
        dataBuffer[0] |= address & AD525X_MEM_ADDR_MASK;
        /* 3 byte data word */
        if((this_device == ID_AD5235) || (this_device == ID_ADN2850))
        {
            SPI_Write(slaveId,
                      dataBuffer,
                      3);
            TIME_DelayUs(50);
            dataBuffer[0] &= AD525X_CMD_NOP << AD525X_CMD_SPI_OFFSET;
            SPI_Read(slaveId,
                            dataBuffer,
                            3);
            data = (dataBuffer[1] << ONEBYTE_OFFSET) | dataBuffer[2];
            return (data & AD525X_DATA10_MASK);
        }
        else /* 2 byte data word */
        {
            SPI_Read(slaveId,
                     dataBuffer,
                     2);
            TIME_DelayUs(50);
            /* Sending a dummy frame to read the result */
            dataBuffer[0] &= AD525X_CMD_NOP << AD525X_CMD_SPI_OFFSET;
            SPI_Read(slaveId,
                     dataBuffer,
                     2);
            data = (unsigned short)dataBuffer[1];
            return data;
        }
    }
    else /* Communication interface is I2C */
    {
        /* Setup I2C address */
        i2cSlaveAddr |= (AD525X_I2C_HARD_ADDR | \
                        (slaveId & AD525X_I2C_PIN_ADDR_MASK));
        dataBuffer[0] &= ~AD525X_I2C_CMD_OR_REG;         // reset CMD/REG_n
        dataBuffer[0] &= ~AD525X_I2C_EE_OR_RDAC;         // reset EE/RDAC_n
        dataBuffer[0] |= address & AD525X_MEM_ADDR_MASK; // set address
        /* Dummy write to select the desired register */
        I2C_Write(i2cSlaveAddr,
                  dataBuffer,
                  1,
                  1);
        dataBuffer[0] &= AD525X_CMD_NOP;
        I2C_Read(i2cSlaveAddr,
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
 * @param + slaveId - slave device ID in case of SPI interface, and
 *                    programmable device address in case of the I2C
 *        + address - desired address of the RDAC register
 *        + data - the data which will be written to the RDAC register
 *
 * @return success
******************************************************************************/
void AD525X_WriteRdac(unsigned char slaveId,
                      unsigned char address,
                      unsigned short data)
{
    unsigned char dataBuffer[3] = {0,};
    unsigned char i2cSlaveAddr = 0;

    if(AD525X_CHIP_INFO[this_device].comm_type == SPI)
    {
        /* Sending the command, reading the result on the next frame */
        dataBuffer[0] |= AD525X_CMD_SPI_SREG2RDAC << AD525X_CMD_SPI_OFFSET;
        dataBuffer[0] |= address & AD525X_MEM_ADDR_MASK;
        if((this_device == ID_AD5235) || (this_device == ID_ADN2850)) /* 3 byte data word */
        {
            dataBuffer[1] = (data & MSB_BYTE_MASK) >> ONEBYTE_OFFSET;
            dataBuffer[2] = data & LSB_BYTE_MASK;
            SPI_Write(slaveId,
                      dataBuffer,
                      3);
        }
        else /* 2 byte data word */
        {
            dataBuffer[1] = data & LSB_BYTE_MASK;
            SPI_Write(slaveId,
                      dataBuffer,
                      2);
        }
    }
    else /* Communication interface is I2C */
    {
        /* Setup I2C address */
        i2cSlaveAddr |= (AD525X_I2C_HARD_ADDR | \
                        (slaveId & AD525X_I2C_PIN_ADDR_MASK));
        dataBuffer[0] &= ~AD525X_I2C_CMD_OR_REG;         // reset CMD/REG_n
        dataBuffer[0] &= ~AD525X_I2C_EE_OR_RDAC;         // reset EE/RDAC_n
        dataBuffer[0] |= address & AD525X_MEM_ADDR_MASK; // set address

        dataBuffer[1] = (data & LSB_BYTE_MASK);
        I2C_Write(i2cSlaveAddr,
                  dataBuffer,
                  2,
                  1);
    }
    TIME_DelayMs(25);
}

/**************************************************************************//**
 * @brief Write quick commands to the device.
 *
 * @param + slaveId - slave device ID in case of SPI interface, and
 *                    programmable device address in case of the I2C
 *        + command - desired command, the functionality of a command it may
 *                    vary at diffrent devices
 *
 * @return success
******************************************************************************/
void AD525X_WriteCommand(unsigned char slaveId,
                         unsigned char command,
                         unsigned char address)
{
    unsigned char dataBuffer[3] = {0,};
    unsigned char i2cSlaveAddr = 0;

    if(AD525X_CHIP_INFO[this_device].comm_type == SPI)
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

        if((this_device == ID_AD5235) || (this_device == ID_ADN2850)) /* 3 byte data word */
        {
            SPI_Write(slaveId,
                      dataBuffer,
                      3);
            if(command == AD525X_CMD_MEM2RDAC)
            {
                dataBuffer[0] &= AD525X_CMD_NOP;
                SPI_Write(slaveId,
                          dataBuffer,
                          3);
            }
        }
        else /* 2 byte data word */
        {
            SPI_Write(slaveId,
                      dataBuffer,
                      2);
            if(command == AD525X_CMD_MEM2RDAC)
            {
                dataBuffer[0] &= AD525X_CMD_NOP;
                SPI_Write(slaveId,
                          dataBuffer,
                          2);
            }
        }
    }
    else /* Communication interface is I2C */
    {
        /* Setup I2C address */
        i2cSlaveAddr |= (AD525X_I2C_HARD_ADDR | \
                        (slaveId & AD525X_I2C_PIN_ADDR_MASK));

        dataBuffer[0] |= AD525X_I2C_CMD_OR_REG;                // set CMD/REG_n
        dataBuffer[0] |= (command & AD525X_CMD_MASK) << AD525X_CMD_I2C_OFFSET;
        dataBuffer[0] |= address & AD525X_RDAC_ADDR_MASK_3BIT; // set address

        I2C_Write(i2cSlaveAddr,
                  dataBuffer,
                  2,
                  1);
        if(command == AD525X_CMD_MEM2RDAC)
        {
            dataBuffer[0] &= AD525X_CMD_NOP;
            I2C_Write(i2cSlaveAddr,
                      dataBuffer,
                      1,
                      1);
        }
    }
    TIME_DelayMs(25);
}
