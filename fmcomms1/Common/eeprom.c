/**************************************************************************//**
*   @file   eeprom.c
*   @brief  EEPROM Implementation file.
*   @author acozma (andrei.cozma@analog.com)
*
*******************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
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
*******************************************************************************
*   SVN Revision: $WCREV$
******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "i2c.h"
#include "eeprom.h"

/**************************************************************************//**
* @brief Reads data from the selected EEPROM device
*
* @param eepromAddr - I2C address of the EEPROM device
* @param pData - Buffer to store the read data
* @param size - Number of bytes to read
*
* @return Returns -1 in case of error, 0 for success
******************************************************************************/
int32_t EEPROM_Read(uint8_t i2cAddr, uint8_t eepromAddr, 
                    uint8_t* pData, uint16_t size)
{
    uint32_t ret;

    ret = I2C_Read(i2cAddr, eepromAddr, 
                   size, pData);

    return (size != ret ? -1 : 0);
}

/**************************************************************************//**
* @brief Writes data to the selected EEPROM device
*
* @param eepromAddr - I2C address of the EEPROM device
* @param pData - Buffer to store the write data
* @param size - Number of bytes to write
*
* @return Returns -1 in case of error, 0 for success
******************************************************************************/
int32_t EEPROM_Write(uint8_t i2cAddr, uint8_t eepromAddr, 
                     uint8_t* pData, uint16_t size)
{
    uint32_t ret;

    ret = I2C_Write(i2cAddr, eepromAddr, 
                    size, pData);

    return (size != ret ? -1 : 0);
}

/**************************************************************************//**
* @brief Reads the calibration data from the calibration EEPROM
*
* @param pData - Pointer to the buffer where to store the calibration data
* @param pSize - Pointer to to a variable where to store the number of
*				 existing calibration records
*
* @return Returns -1 in case of error, 0 for success
******************************************************************************/
int32_t EEPROM_GetCalData(uint8_t* pData, uint8_t *pSize)
{
    int32_t ret;
    struct fmcomms1_calib_data* pCalData;

    *pSize = 0;
    ret = EEPROM_Read(IICSEL_CAL, 0x00, 
                      pData, MAX_SIZE_CAL_EEPROM);
    if(ret < 0)
        return ret;

    pCalData = (struct fmcomms1_calib_data*)pData;
    
    if ((pCalData->adi_magic0 == ADI_MAGIC_0) && 
        (pCalData->adi_magic1 == ADI_MAGIC_1) /*&&
        (pCalData->version == ADI_VERSION(VERSION_SUPPORTED)*/)
    {
        (*pSize)++;
    }

    while(pCalData->next)
    {
        (*pSize)++;
		pCalData++;
    }

    return ret;
}
