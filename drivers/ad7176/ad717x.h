/**************************************************************************//**
*   @file    AD7176.h
*   @brief   AD7176 header file.
*   @devices AD7172-2, AD7172-4, AD7173-8, AD7175-2, AD7175-8, AD7176-2
*   @author  acozma (andrei.cozma@analog.com)
*            dnechita (dan.nechita@analog.com)
*******************************************************************************
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
******************************************************************************/

#ifndef __AD7176_H__
#define __AD7176_H__

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdint.h>
#include "AD7176_regs.h"

typedef enum ad7176_crc_mode ad7176_crc_mode;

enum ad7176_crc_mode
{
	AD7176_DISABLE,
	AD7176_USE_CRC,
	AD7176_USE_XOR,
};

/*
 * The structure describes the device and is used with the ad7176 driver.
 * @slave_select_id: The ID of the Slave Select to be passed to the SPI calls.
 * @regs: A reference to the register list of the device that the user must
 *       provide when calling the Setup() function.
 * @userCRC: Error check type to use on SPI transfers.
 */
struct ad7176_device {
	int slave_select_id;
	ad7176_st_reg *regs;
	ad7176_crc_mode useCRC;
};

/*****************************************************************************/
/******************* AD7176 Constants ****************************************/
/*****************************************************************************/
#define AD7176_CRC8_POLYNOMIAL_REPRESENTATION 0x07 /* x8 + x2 + x + 1 */

/*****************************************************************************/
/************************ Functions Declarations *****************************/
/*****************************************************************************/
/*! Reads the value of the specified register. */
int32_t AD7176_ReadRegister(struct ad7176_device *device, ad7176_st_reg* pReg);

/*! Writes the value of the specified register. */
int32_t AD7176_WriteRegister(struct ad7176_device *device, ad7176_st_reg reg);

/*! Resets the device. */
int32_t AD7176_Reset(struct ad7176_device *device);

/*! Waits until a new conversion result is available. */
int32_t AD7176_WaitForReady(struct ad7176_device *device, uint32_t timeout);

/*! Reads the conversion result from the device. */
int32_t AD7176_ReadData(struct ad7176_device *device, int32_t* pData);

/*! Computes the CRC checksum for a data buffer. */
uint8_t AD7176_ComputeCRC8(uint8_t* pBuf, uint8_t bufSize);

/*! Computes the XOR checksum for a data buffer. */
uint8_t AD7176_ComputeXOR8(uint8_t * pBuf, uint8_t bufSize);

/*! Updates the CRC settings. */
int32_t AD7176_UpdateCRCSetting(struct ad7176_device *device);

/*! Initializes the AD7176. */
int32_t AD7176_Setup(struct ad7176_device *device, int slave_select,
			ad7176_st_reg *regs);

#endif /* __AD7176_H__ */
