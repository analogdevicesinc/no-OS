/**************************************************************************//**
*   @file   i2c_ps7.h
*   @brief  ZYNQ Hardware I2C header file.
*   @author ATofan (alexandru.tofan@analog.com)
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

#ifndef __I2C_PS7_H__
#define __I2C_PS7_H__

/*****************************************************************************/
/******************* Include Files *******************************************/
/*****************************************************************************/
#include <stdint.h>
#include "xparameters.h"
#include "system_config.h"

#if(USE_PS7 == 0)

#define I2C_BASEADDR				XPS_I2C1_BASEADDR

/* ZYNQ Hardware I2C Registers */
#define HW_I2C_CONTROL_REG          0x00
#define HW_I2C_STATUS_REG           0x04
#define HW_I2C_ADDRESS_REG          0x08
#define HW_I2C_DATA_REG             0x0C
#define HW_I2C_INTR_STATUS_REG      0x10
#define HW_I2C_TX_SIZE_REG          0x14
#define HW_I2C_SLAVE_MON_PAUSE_REG  0x18
#define HW_I2C_TIMEOUT_REG          0x1C
#define HW_I2C_INTR_MSK_REG         0x20
#define HW_I2C_INTR_EN_REG          0x24
#define HW_I2C_INTR_DIS_REG         0x28

/* ZYNQ Hardware I2C Register Specific Bits */
// CONTROL REG 0x00
#define DIV_A                       14
#define DIV_B                       8
#define CLR_FIFO                    6
#define SLVMON                      5
#define HOLD                        4
#define ACK_EN                      3
#define NEA                         2
#define MS                          1
#define RW                          0
// STATUS REG 0x04
#define BA                          8
#define RXOVF                       7
#define TXDV                        6
#define RXDV                        5
#define RXRW                        3
// ADDRESS REG 0x08
#define ADD_MSK                     0       
// DATA REG 0x0C    
#define DATA_MSK                    0       
// INTERRUPT STATUS REG 0x10
#define ARB_LOST                    9
#define RX_UNF                      7
#define TX_OVF                      6
#define RX_OVF                      5
#define SLV_RDY                     4
#define IXR_TO                      3
#define IXR_NACK                    2
#define IXR_DATA                    1
#define IXR_COMP                    0
// TRANSFER SIZE REG 0x14
#define Transfer_Size               0
// SLAVE MON PAUSE REG 0x18
#define Pause_MASK                  0
// TIMEOUT REG 0x1C
#define TO_MASK                     0
// INTERRUPT MASK REG 0x20 - same bits as INTERRUPT STATUS REG
// INTERRUPT ENABLE REG 0x24 - same bits as INTERRUPT STATUS REG
// INTERRUPT DISABLE REG 0x28 - same bits as INTERRUPT STATUS REG

/*****************************************************************************/
/************************ Functions Declarations *****************************/
/*****************************************************************************/

uint32_t I2C_Init_ps7();
uint32_t I2C_Read_ps7(int i2cBaseAddr, uint32_t i2cAddr, uint32_t regAddr, uint32_t rxSize, uint8_t* rxBuf);
uint32_t I2C_Write_ps7(int i2cBaseAddr, uint32_t i2cAddr, uint32_t regAddr, uint32_t txSize, uint8_t* txBuf);

#endif

#endif /* __I2C_PS7_H__ */
