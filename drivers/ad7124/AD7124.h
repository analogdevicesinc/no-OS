/***************************************************************************//**
*   @file    AD7124.h
*   @brief   AD7124 header file.
*   @devices AD7124-4, AD7124-8
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

#ifndef __AD7124_H__
#define __AD7124_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "AD7124_regs.h"

typedef struct ad7124_device ad7124_device;

/*
 * The structure describes the device and is used with the ad7124 driver.
 * @slave_select_id: The ID of the Slave Select to be passed to the SPI calls.
 * @regs: A reference to the register list of the device that the user must
 *       provide when calling the Setup() function.
 * @userCRC: Whether to do or not a cyclic redundancy check on SPI transfers.
 * @check_ready: When enabled all register read and write calls will first wait
 *               until the device is ready to accept user requests.
 * @spi_rdy_poll_cnt: Number of times the driver should read the Error register
 *                    to check if the device is ready to accept user requests,
 *                    before a timeout error will be issued.
 */
struct ad7124_device {
	int slave_select_id;
	ad7124_st_reg *regs;
	int useCRC;
	int check_ready;
	int spi_rdy_poll_cnt;
};

/******************************************************************************/
/******************* AD7124 Constants *****************************************/
/******************************************************************************/
#define AD7124_CRC8_POLYNOMIAL_REPRESENTATION 0x07 /* x8 + x2 + x + 1 */
#define AD7124_DISABLE_CRC 0
#define AD7124_USE_CRC 1

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/*! Reads the value of the specified register. */
int32_t AD7124_ReadRegister(ad7124_device *device, ad7124_st_reg* pReg);

/*! Writes the value of the specified register. */
int32_t AD7124_WriteRegister(ad7124_device *device, ad7124_st_reg reg);

/*! Reads the value of the specified register without a device state check. */
int32_t AD7124_NoCheckReadRegister(ad7124_device *device, ad7124_st_reg* pReg);

/*! Writes the value of the specified register without a device state check. */
int32_t AD7124_NoCheckWriteRegister(ad7124_device *device, ad7124_st_reg reg);

/*! Resets the device. */
int32_t AD7124_Reset(ad7124_device *device);

/*! Waits until the device can accept read and write user actions. */
int32_t AD7124_WaitForSpiReady(ad7124_device *device, uint32_t timeout);

/*! Waits until a new conversion result is available. */
int32_t AD7124_WaitForConvReady(ad7124_device *device, uint32_t timeout);

/*! Reads the conversion result from the device. */
int32_t AD7124_ReadData(ad7124_device *device, int32_t* pData);

/*! Computes the CRC checksum for a data buffer. */
uint8_t AD7124_ComputeCRC8(uint8_t* pBuf, uint8_t bufSize);

/*! Updates the CRC settings. */
void AD7124_UpdateCRCSetting(ad7124_device *device);

/*! Updates the device SPI interface settings. */
void AD7124_UpdateDevSpiSettings(ad7124_device *device);

/*! Initializes the AD7124. */
int32_t AD7124_Setup(ad7124_device *device, int slave_select,
			ad7124_st_reg *regs);

#endif /* __AD7124_H__ */
