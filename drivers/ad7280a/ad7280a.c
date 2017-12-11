/**************************************************************************//**
*   @file   ad7280a.c
*   @brief  Driver for the AD7280A Lithium Ion Battery Monitoring System
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2014(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include "platform_drivers.h"
#include "ad7280a.h"

/*****************************************************************************/
/************************ Functions Definitions ******************************/
/*****************************************************************************/

/******************************************************************************
 * @brief Initializes the communication with the device.
 *
 * @param device     - The device structure.
 *        init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
******************************************************************************/
char AD7280A_Init(ad7280a_dev **device,
		  ad7280a_init_param init_param)
{
	ad7280a_dev *dev;
    char          status;
    unsigned long value;

	dev = (ad7280a_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* GPIO */
	status = gpio_get(&dev->gpio_pd, init_param.gpio_pd);
	status |= gpio_get(&dev->gpio_cnvst, init_param.gpio_cnvst);
	status |= gpio_get(&dev->gpio_alert, init_param.gpio_alert);

    AD7280A_PD_OUT;
    AD7280A_PD_HIGH;
    AD7280A_CNVST_OUT;
    AD7280A_CNVST_HIGH;
    AD7280A_ALERT_IN;

    /* Wait 250us */
    mdelay(250);

    status |= spi_init(&dev->spi_desc, init_param.spi_init);

    /* Example 1 from the datasheet */
    /* Configure the Control LB register for all devices */
    value = AD7280A_CRC_Write((unsigned long) (AD7280A_CONTROL_LB << 21) |
                              ((AD7280A_CTRL_LB_MUST_SET |
                              AD7280A_CTRL_LB_LOCK_DEV_ADDR |
                              AD7280A_CTRL_LB_DAISY_CHAIN_RB_EN) << 13) |
                              (1 << 12));
    AD7280A_Transfer_32bits(dev,
			    value);
    /* Configure the Read Register for all devices */
    value = AD7280A_CRC_Write((unsigned long) (AD7280A_READ << 21) |
                              (AD7280A_CONTROL_LB << 15) |
                              (1 << 12));
    AD7280A_Transfer_32bits(dev,
			    value);
    /* Read master address */
    value = AD7280A_Transfer_32bits(dev,
				    AD7280A_READ_TXVAL);
    //printf("Master address=0x%x\r\n",(value >> 27));
    /* Read slave address */
    value = AD7280A_Transfer_32bits(dev,
				    AD7280A_READ_TXVAL);
    //printf("Slave address=0x%x\r\n",(value >> 27));

	*device = dev;

    return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by AD7280A_Init().
 *
 * @param dev - The device structure.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t AD7280A_remove(ad7280a_dev *dev)
{
	int32_t ret;

	ret = spi_remove(dev->spi_desc);

	ret |= gpio_remove(dev->gpio_pd);
	ret |= gpio_remove(dev->gpio_cnvst);
	ret |= gpio_remove(dev->gpio_alert);

	free(dev);

	return ret;
}

/******************************************************************************
* @brief Reads/transmits 32 data bits from/to AD7280A.
 *
 * @param dev           - The device structure.
 *        data          - Data to be transmitted (MOSI pin).
 *
 * @return receivedData - Received data (MISO pin).
******************************************************************************/
unsigned long AD7280A_Transfer_32bits(ad7280a_dev *dev,
				      unsigned long data)
{
    unsigned char dataBuf[4];
    unsigned long receivedData;

    dataBuf[0] = (data >> 24) & 0xff;
    dataBuf[1] = (data >> 16) & 0xff;
    dataBuf[2] = (data >> 8)  & 0xff;
    dataBuf[3] = (data >> 0)  & 0xff;

	spi_write_and_read(dev->spi_desc,
			   (unsigned char*)dataBuf,
			   4);

    receivedData = ((unsigned long)dataBuf[0] << 24) +
                   ((unsigned long)dataBuf[1] << 16) +
                   ((unsigned long)dataBuf[2] << 8)  +
                   ((unsigned long)dataBuf[3] << 0);

    return receivedData;
}

/******************************************************************************
 * @brief Computes the CRC value for a write transmission, and prepares the
 *        complete write codeword
 *
 * @param message : The message that is to be transmitted. The least significant
 *                  11 bits are discarded
 *
 * @return Complete codeword that can be transmitted to AD7280A
******************************************************************************/
unsigned long AD7280A_CRC_Write(unsigned long message)
{
    long            i;
    long            CRC;
    long            xor1 = 0;
    long            xor2 = 0;
    long            xor3 = 0;
    long            xor4 = 0;
    long            xor5 = 0;
    unsigned long   data_out = 0;
    long            CRC0 = 0;
    long            CRC1 = 0;
    long            CRC2 = 0;
    long            CRC3 = 0;
    long            CRC4 = 0;
    long            CRC5 = 0;
    long            CRC6 = 0;
    long            CRC7 = 0;

    message = message >> 11;
    for (i = NUMBITS_WRITE; i >= 0; i--)
    {
        xor5 = (CRC4 ^ CRC7) ? 1 : 0;
        xor4 = (CRC2 ^ CRC7) ? 1 : 0;
        xor3 = (CRC1 ^ CRC7) ? 1 : 0;
        xor2 = (CRC0 ^ CRC7) ? 1 : 0;
        xor1 = ((message & (1 << i)) ? 1 : 0) ^ (CRC7 ? 1 : 0);
        CRC7 = CRC6;
        CRC6 = CRC5;
        CRC5 = xor5;
        CRC4 = CRC3;
        CRC3 = xor4;
        CRC2 = xor3;
        CRC1 = xor2;
        CRC0 = xor1;
    }
    CRC = (CRC0 ? (1 << 0) : 0) | (CRC1 ? (1 << 1) : 0) | (CRC2 ? (1 << 2) : 0) |
          (CRC3 ? (1 << 3) : 0) | (CRC4 ? (1 << 4) : 0) | (CRC5 ? (1 << 5) : 0) |
          (CRC6 ? (1 << 6) : 0) | (CRC7 ? (1 << 7) : 0);
    data_out = (message << 11) | (CRC << 3) | 2;

    return data_out;
}

/******************************************************************************
 * @brief Checks the received message if the received CRC and computed CRC are
 *        the same.
 *
 * @param message : The received message
 *
 * @return 1 if the two CRC are identical
 *         0 otherwise
******************************************************************************/
long AD7280A_CRC_Read(unsigned long message)
{
    long            i;
    unsigned long   data_in=0;
    long            CRC_REC;
    long            CRC_COMPUTED;
    long            xor1 = 0;
    long            xor2 = 0;
    long            xor3 = 0;
    long            xor4 = 0;
    long            xor5 = 0;
    long            CRC0 = 0;
    long            CRC1 = 0;
    long            CRC2 = 0;
    long            CRC3 = 0;
    long            CRC4 = 0;
    long            CRC5 = 0;
    long            CRC6 = 0;
    long            CRC7 = 0;

    CRC_REC= (message >> 2) & 0xFF;
    data_in = message >> 10;
    for (i = NUMBITS_READ; i >= 0; i--)
    {
        xor5 = (CRC4 ^ CRC7) ? 1 : 0;
        xor4 = (CRC2 ^ CRC7) ? 1 : 0;
        xor3 = (CRC1 ^ CRC7) ? 1 : 0;
        xor2 = (CRC0 ^ CRC7) ? 1 : 0;
        xor1 = ((data_in & (1 << i)) ? 1 : 0) ^ (CRC7 ? 1 : 0);
        CRC7 = CRC6;
        CRC6 = CRC5;
        CRC5 = xor5;
        CRC4 = CRC3;
        CRC3 = xor4;
        CRC2 = xor3;
        CRC1 = xor2;
        CRC0 = xor1;
    }
    CRC_COMPUTED = (CRC0 ? (1 << 0) : 0) | (CRC1 ? (1 << 1) : 0) |
                   (CRC2 ? (1 << 2) : 0) | (CRC3 ? (1 << 3) : 0) |
                   (CRC4 ? (1 << 4) : 0) | (CRC5 ? (1 << 5) : 0) |
                   (CRC6 ? (1 << 6) : 0) | (CRC7 ? (1 << 7) : 0);
    if (CRC_REC == CRC_COMPUTED)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/******************************************************************************
 * @brief Performs a read from all registers on 2 devices.
 *
 * @param dev - The device structure.
 *
 * @return 1.
******************************************************************************/
char AD7280A_Convert_Read_All(ad7280a_dev *dev)
{
    unsigned char i;
    //long err = 1;
    unsigned long value;

    /* Configure Control HB register. Read all register, convert all registers,
    average 8 values for all devices */
    value = AD7280A_CRC_Write((unsigned long) (AD7280A_CONTROL_HB << 21) |
                             ((AD7280A_CTRL_HB_CONV_RES_READ_ALL |
                               AD7280A_CTRL_HB_CONV_INPUT_ALL |
                               AD7280A_CTRL_HB_CONV_AVG_8) << 13) |
                                (1 << 12));
    /* Configure the Read register for all devices */
    AD7280A_Transfer_32bits(dev,
			    value);
    value = AD7280A_CRC_Write((unsigned long) (AD7280A_READ << 21) |
                              (AD7280A_CELL_VOLTAGE_1 << 15) |
                              (1 << 12));
    AD7280A_Transfer_32bits(dev,
			    value);
    /* Configure the CNVST register, allow single CNVST pulse */
    value = AD7280A_CRC_Write((unsigned long) (AD7280A_CNVST_N_CONTROL << 21) |
                              (2 << 13) |
                              (1 << 12));
    AD7280A_Transfer_32bits(dev,
			    value);
    /* Wait 100us */
    mdelay(100);
    /* Toggle CNVST pin */
    AD7280A_CNVST_LOW;
    /* Wait 50us */
    mdelay(50);
    AD7280A_CNVST_HIGH;
    /* Wait 300us */
    mdelay(300);
    /* Read data from both devices */
    for(i = 0; i < 24; i++)
    {
        dev->readData[i] = AD7280A_Transfer_32bits(dev,
					      AD7280A_READ_TXVAL);
    }

    // Optionally check all received data to make sure CRC is correct
    /*for(i=0;i<24;i++)
    {
        if ( crc_read(dev->readData[i]) == 0 )
        {
            err = 0;
        }
    }*/

    /* Convert the received data to float values. */
    AD7280A_Convert_Data_All(dev);

    return (1);
}

/******************************************************************************
 * @brief Converts acquired data from all channels to float values.
 *
 * @param dev - The device structure.
 *
 * @return 1.
******************************************************************************/
char AD7280A_Convert_Data_All(ad7280a_dev *dev)
{
    unsigned char i;

    for(i = 0; i < 6; i++)
    {
        dev->cellVoltage[i]     = 1 + ((dev->readData[i]    >> 11) & 0xfff) * 0.0009765625;
        dev->auxADC[i]          = ((dev->readData[i+6]      >> 11) & 0xfff) * 0.001220703125;
        dev->cellVoltage[i + 6] = 1 + ((dev->readData[i+12] >> 11) & 0xfff) * 0.0009765625;
        dev->auxADC[i + 6]      = ((dev->readData[i+18]     >> 11) & 0xfff) * 0.001220703125;
    }

    return (1);
}

/******************************************************************************
 * @brief Reads the register content of one selected register.
 *
 * @param  dev     - The device structure.
 *         devAddr - the device address
 *                   Example: 0 - master.
 *                            1 - slave.
 *         readReg - the address of the register to be read from the
 *                   selected device (only registers from Register Map with
 *                   address higher or equal to 0x0D).
 *
 * @return readContent - register content.
******************************************************************************/
short AD7280A_Read_Register(ad7280a_dev *dev,
			    unsigned char devAddr,
                            unsigned char readReg)
{
     unsigned long value;
     short         readContent;
     //unsigned char readDevAddress;
     //unsigned char readRegAddress;
     //unsigned char readWriteAck;


      /* Example 6 from datasheet */
      /* Disable reading on all devices */
      value = AD7280A_CRC_Write((unsigned long) (AD7280A_CONTROL_HB << 21) |
                                 (AD7280A_CTRL_HB_CONV_RES_READ_NO << 13) |
                                 (1 << 12));
      AD7280A_Transfer_32bits(dev,
			      value);
      /* Enable reading on the selected device */
      value = AD7280A_CRC_Write((unsigned long) (devAddr << 31) |
                                (AD7280A_CONTROL_HB << 21) |
                                ((AD7280A_CTRL_HB_CONV_RES_READ_ALL |
                                AD7280A_CTRL_HB_CONV_INPUT_ALL) << 13));
      AD7280A_Transfer_32bits(dev,
			      value);
      /* Wait 100us */
      mdelay(100);
      /* Configure the Read register */
      value = AD7280A_CRC_Write((unsigned long) (devAddr << 31) |
                                 (AD7280A_READ << 21) |
                                 (readReg << 15));
      AD7280A_Transfer_32bits(dev,
			      value);
      /* Perform the read */
      value = AD7280A_Transfer_32bits(dev,
				      AD7280A_READ_TXVAL);
      if (AD7280A_CRC_Read(value) == 1)
      {
          //readDevAddress = value >> 27;              // Extract the Device address
          //readRegAddress = (value >> 21 ) & 0x2F;    // Extract the Register address
          readContent    = (value >> 13 ) & 0xFF;    // Extract the Register data
          //readWriteAck   = (value & 0x200) ? 1 : 0;  // Extract the write acknowledge
      }
      else
      {
          readContent = -1;
      }

     return readContent;
}

/******************************************************************************
 * @brief Reads the conversion of one channel.
 *
 * @param  dev     - The device structure.
 *         devAddr - the device address
 *                   Example: 0 - master.
 *                            1 - slave.
 *         readReg - the address of the register to be read from
 *                   selected device (only Cell_Voltage_x or AUX_ADC_x).
 *
 * @return readConversion - ADC code or -1 if an error occured
******************************************************************************/
short AD7280A_Read_Conversion(ad7280a_dev *dev,
			      unsigned char devAddr,
                              unsigned char readReg)
{
    unsigned long  value;
    short          readConversion;
    //unsigned char  readDevAddress;
    //unsigned char  readRegAddress;
    //unsigned char  readWriteAck;

    /* Example 4 from datasheet */
    /* Write the register address to Read Register */
    value = AD7280A_CRC_Write((unsigned long)(devAddr << 31) |
                              (AD7280A_READ << 21) |
                              (readReg << 15));
    AD7280A_Transfer_32bits(dev,
			    value);
    /* Disable reading on all devices */
    value = AD7280A_CRC_Write((unsigned long)(AD7280A_CONTROL_HB << 21) |
                              (AD7280A_CTRL_HB_CONV_RES_READ_NO << 13) |
                              (1 << 12));
    AD7280A_Transfer_32bits(dev,
			    value);
    /* Wait 100us */
    mdelay(100);
    /*  */
    value = AD7280A_CRC_Write((unsigned long)(devAddr << 31) |
                              (AD7280A_CONTROL_HB << 21) |
                              ((AD7280A_CTRL_HB_CONV_RES_READ_ALL |
                               AD7280A_CTRL_HB_CONV_INPUT_ALL) << 13));
    AD7280A_Transfer_32bits(dev,
			    value);
    /* Wait 100us */
    mdelay(100);
    /* Allow conversions to be initiated using CNVST pin on selected part */
    value=AD7280A_CRC_Write((unsigned long)(devAddr << 31) |
                            (AD7280A_CNVST_N_CONTROL << 21) |
                            (2 << 13));
    /* Write the CNVST_N register, allow single CNVST pulse */
    AD7280A_Transfer_32bits(dev,
			    value);
    /* Initiate conversions through the falling edge of CNVST */
    AD7280A_CNVST_LOW;
    /* Allow sufficient time for all conversions to be completed */
    /* Wait 50us */
    mdelay(50);
    AD7280A_CNVST_HIGH;
    /* Wait 300us */
    mdelay(300);
    /* Perform the read */
    value = AD7280A_Transfer_32bits(dev,
				    AD7280A_READ_TXVAL);

    if (AD7280A_CRC_Read(value) == 1)
    {
        //readDevAddress = value >> 27;              // Extract the Device address
        //readRegAddress = (value >> 23 ) & 0x2F;    // Extract the Register address
        readConversion = (value >> 11 ) & 0xFFF;   // Extract the Register data
        //readWriteAck   = (value & 0x200) ? 1 :0;   //Extract the write acknowledge
    }
    else
    {
        readConversion = -1;
    }

    return readConversion;
}

/******************************************************************************
 * @brief Converts the input data to a voltage value.
 *
 * @param  type - 0 = cell voltage input calculation.
 *              - 1 = auxiliary ADC input calculation.
 *         data - ADC code to be converted to volts.
 *
 * @return result - calculated voltage.
******************************************************************************/
float AD7280A_Convert_Data(unsigned char type,
                           unsigned short data)
{
    float result = 0;

    data = data & 0xfff;
    if (type == 0)
    {
        result = 1 + ((float)data * 0.0009765625);
    }
    else
    {
        if (type == 1)
        {
            result = (float)data * 0.001220703125;
        }
    }

    return result;
}

/******************************************************************************
 * @brief Writes the content of one selected register from the selected device.
 *
 * @param  dev     - The device structure.
 *         devAddr - the device address
 *                   Example: 0 - master.
 *                            1 - slave.
 *         readReg - the address of the register to be read from the
 *                   selected device (only registers from Register Map with
 *                   address higher or equal to 0x0D).
 *
 * @return none.
******************************************************************************/
void AD7280A_Write_Register(ad7280a_dev *dev,
			    unsigned char devAddr,
                            unsigned char readReg,
                            unsigned char regVal)
{
    unsigned long value = 0;

    switch (readReg)
    {
        case 0x0D:
            value = AD7280A_CRC_Write((unsigned long) (devAddr << 31) |
                                      (AD7280A_CONTROL_HB << 21)      |
                                      (regVal << 13));
            AD7280A_Transfer_32bits(dev,
				    value);
            break;
        case 0x0E:
            value = AD7280A_CRC_Write((unsigned long) (devAddr << 31) |
                                      (AD7280A_CONTROL_LB << 21)      |
                                      (regVal << 13));
            AD7280A_Transfer_32bits(dev,
				    value);
            break;
        case 0x0F:
            value = AD7280A_CRC_Write((unsigned long) (devAddr << 31)  |
                                      (AD7280A_CELL_OVERVOLTAGE << 21) |
                                      (regVal << 13));
            AD7280A_Transfer_32bits(dev,
				    value);
            break;
        case 0x10:
            value = AD7280A_CRC_Write((unsigned long) (devAddr << 31)   |
                                      (AD7280A_CELL_UNDERVOLTAGE << 21) |
                                      (regVal << 13));
            AD7280A_Transfer_32bits(dev,
				    value);
            break;
        case 0x11:
            value = AD7280A_CRC_Write((unsigned long) (devAddr << 31)     |
                                      (AD7280A_AUX_ADC_OVERVOLTAGE << 21) |
                                      (regVal << 13));
            AD7280A_Transfer_32bits(dev,
				    value);
            break;
        case 0x12:
            value = AD7280A_CRC_Write((unsigned long) (devAddr << 31)      |
                                      (AD7280A_AUX_ADC_UNDERVOLTAGE << 21) |
                                      (regVal << 13));
            AD7280A_Transfer_32bits(dev,
				    value);
            break;
        case 0x13:
            value = AD7280A_CRC_Write((unsigned long) (devAddr << 31) |
                                      (AD7280A_ALERT << 21)           |
                                      (regVal << 13));
            AD7280A_Transfer_32bits(dev,
				    value);
            break;
        case 0x14:
            value = AD7280A_CRC_Write((unsigned long) (devAddr << 31) |
                                      (AD7280A_CELL_BALANCE << 21)    |
                                      (regVal << 13));
            AD7280A_Transfer_32bits(dev,
				    value);
            break;
        case 0x15:
            value = AD7280A_CRC_Write((unsigned long) (devAddr << 31) |
                                      (AD7280A_CB1_TIMER << 21)       |
                                      (regVal << 13));
            AD7280A_Transfer_32bits(dev,
				    value);
            break;
        case 0x16:
            value = AD7280A_CRC_Write((unsigned long) (devAddr << 31) |
                                      (AD7280A_CB2_TIMER << 21)       |
                                      (regVal << 13));
            AD7280A_Transfer_32bits(dev,
				    value);
            break;
        case 0x17:
            value = AD7280A_CRC_Write((unsigned long) (devAddr << 31) |
                                      (AD7280A_CB3_TIMER << 21)       |
                                      (regVal << 13));
            AD7280A_Transfer_32bits(dev,
				    value);
            break;
        case 0x18:
            value = AD7280A_CRC_Write((unsigned long) (devAddr << 31) |
                                      (AD7280A_CB4_TIMER << 21)       |
                                      (regVal << 13));
            AD7280A_Transfer_32bits(dev,
				    value);
            break;
        case 0x19:
            value = AD7280A_CRC_Write((unsigned long) (devAddr << 31) |
                                      (AD7280A_CB5_TIMER << 21)       |
                                      (regVal << 13));
            AD7280A_Transfer_32bits(dev,
				    value);
            break;
        case 0x1A:
            value = AD7280A_CRC_Write((unsigned long) (devAddr << 31) |
                                      (AD7280A_CB6_TIMER << 21)       |
                                      (regVal << 13));
            AD7280A_Transfer_32bits(dev,
				    value);
            break;
        case 0x1B:
            value = AD7280A_CRC_Write((unsigned long) (devAddr << 31) |
                                      (AD7280A_PD_TIMER << 21)        |
                                      (regVal << 13));
            AD7280A_Transfer_32bits(dev,
				    value);
            break;
        case 0x1C:
            value = AD7280A_CRC_Write((unsigned long) (devAddr << 31) |
                                      (AD7280A_READ << 21)            |
                                      (regVal << 13));
            AD7280A_Transfer_32bits(dev,
				    value);
            break;
        case 0x1D:
            value = AD7280A_CRC_Write((unsigned long) (devAddr << 31) |
                                      (AD7280A_CNVST_N_CONTROL << 21) |
                                      (regVal << 13));
            AD7280A_Transfer_32bits(dev,
				    value);
            break;
    }
}

/******************************************************************************
 * @brief Performs the self test for two devices(one master and one slave).
 *
 * @param   dev           - The device structure.
 *          *selfTestRegA - the voltage corresponding to self test master
 *                          register.
 *          *selfTestRegB - the voltage corresponding to self test slave
 *                          register.
 *
 * @return none.
******************************************************************************/
void AD7280A_Selftest_All(ad7280a_dev *dev,
			  float *selfTestRegA,
			  float *selfTestRegB)
{
    unsigned long value;

    /* Example 7 of AD7280 datasheet */
    value = AD7280A_CRC_Write((unsigned long) (AD7280A_CONTROL_HB << 21)    |
                              ((AD7280A_CTRL_HB_CONV_RES_READ_ALL           |
                               AD7280A_CTRL_HB_CONV_INPUT_SELF_TEST) << 13) |
                               (1 << 12));
    /* Select self test conversion and read on all parts */
    AD7280A_Transfer_32bits(dev,
			    value);
    /* Wait 100us */
    mdelay(100);
    value = AD7280A_CRC_Write((unsigned long) (AD7280A_READ << 21) |
                              (AD7280A_SELF_TEST << 15)            |
                              (1 << 12));
    /* Write the read register with the address of the Self test register */
    AD7280A_Transfer_32bits(dev,
			    value);
    value = AD7280A_CRC_Write((unsigned long) (AD7280A_CNVST_N_CONTROL << 21) |
                                              (2 << 13)                       |
                                              (1 << 12));
    /* Write the CNVST_N register, allow single CNVST pulse */
    AD7280A_Transfer_32bits(dev,
			    value);
    AD7280A_CNVST_LOW;
    /* wait 100us */
    mdelay(100);
    AD7280A_CNVST_HIGH;
    /* wait 300us */
    mdelay(300);
    value = AD7280A_CRC_Write((unsigned long) (AD7280A_CNVST_N_CONTROL << 21) |
                                              (1 << 13)                       |
                                              (1 << 12));
    /* Update registers */
    AD7280A_Transfer_32bits(dev,
			    value);
    value = (AD7280A_Transfer_32bits(dev,
				     AD7280A_READ_TXVAL) >> 11) & 0xfff;
    /* Read master self test register */
    *selfTestRegA = (float)value * 0.001220703125;
    value = (AD7280A_Transfer_32bits(dev,
				     AD7280A_READ_TXVAL) >> 11) & 0xfff;
    /* Read slave self test register */
    *selfTestRegB = (float)value * 0.001220703125;
    value = AD7280A_CRC_Write((unsigned long) (AD7280A_CONTROL_HB << 21) |
                              ((AD7280A_CTRL_HB_CONV_RES_READ_ALL        |
                              AD7280A_CTRL_HB_CONV_INPUT_ALL) << 13)     |
                              (1 << 12));
    AD7280A_Transfer_32bits(dev,
			    value);
}

/******************************************************************************
 * @brief Reads the value of Alert Pin from the device.
 *
 * @param dev - The device structure.
 *
 * @return alertAD7280A - the value read of the pin
 *                        Example: 0 - the pin is low.
 *                                 1 - the pin is high.
******************************************************************************/
unsigned char AD7280A_Alert_Pin(ad7280a_dev *dev)
{
    unsigned char alertAD7280A = 0;

	gpio_get_value(dev->gpio_alert,
		       &alertAD7280A);

    return alertAD7280A;
}
