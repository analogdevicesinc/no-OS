/**************************************************************************//**
*   @file   spi_ps7.h
*   @brief  ZYNQ Hardware SPI header file.
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

#ifndef __SPI_PS7_H__
#define __SPI_PS7_H__

/*****************************************************************************/
/******************* Include Files *******************************************/
/*****************************************************************************/
#include <stdint.h>
#include "xil_types.h"
#include "system_config.h"

#if(USE_PS7 == 1)
#define SPI_BASEADDR 	XPAR_PS7_SPI_0_BASEADDR

/* ZYNQ Hardware SPI Registers */
#define HW_SPI_CONFIG_REG           0x00
#define HW_SPI_INTR_STS_REG         0x04
#define HW_SPI_INTR_EN_REG          0x08
#define HW_SPI_INTR_DIS_REG         0x0C
#define HW_SPI_INTR_MASK_REG        0x10
#define HW_SPI_EN_REG               0x14
#define HW_SPI_DELAY_REG            0x18
#define HW_SPI_TXDATA_REG           0x1C
#define HW_SPI_RXDATA_REG           0x20
#define HW_SPI_SLAVE_IDLE_COUNT_REG 0x24
#define HW_SPI_TX_THRES_REG         0x28
#define HW_SPI_RX_THRES_REG         0x2C
#define HW_SPI_MOD_ID_REG           0xFC

/* ZYNQ Hardware SPI Registers */
/* Config Register 0x00 */
#define Modefail_gen_en             17
#define Man_start_com               16
#define Man_start_en                15
#define Manual_CS                   14
#define CS_Bits                     10
#define PERI_SEL                    9
#define SPI_REF_CLK                 8
#define SPI_BAUD_RATE_DIV           3
#define SPI_CLK_PH                  2
#define SPI_CLK_POL                 1
#define SPI_MODE_SEL                0
/* Interrupt Status Register 0x04 */
#define SPI_TX_FIFO_underflow       6
#define SPI_RX_FIFO_full            5
#define SPI_RX_FIFO_not_empty       4
#define SPI_TX_FIFO_full            3
#define SPI_TX_FIFO_not_full        2
#define SPI_MODE_FAIL               1
#define SPI_RX_OVERFLOW             0
/* SPI Enable Register */
#define SPI_EN                      0

u32 SPI_Init_ps7(u32 hwBaseAddr, char lsbFirst, char cpha, char cpol);
u32 SPI_TransferData_ps7(u32 hwBaseAddr, char txSize, char* txBuf, char rxSize, char* rxBuf, char ssNo);

#endif
#endif
