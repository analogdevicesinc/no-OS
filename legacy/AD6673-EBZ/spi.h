/**************************************************************************//**
*   @file   spi.h
*   @brief  SPI header file.
*   @author acozma (andrei.cozma@analog.com)
*
*******************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __SPI__H__
#define __SPI_H__

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "xil_types.h"

/*****************************************************************************/
/******************* SPI Registers Definitions *******************************/
/*****************************************************************************/
// Register address
#define SRR         0x40
#define SPICR       0x60
#define SPISR       0x64
#define SPIDTR      0x68
#define SPIDRR      0x6C
#define SPISSR      0x70
#define SPI_T_FIFO  0x74
#define SPI_R_FIFO  0x78
#define DGIER       0x1C
#define IPISR       0x20
#define IPIER       0x28

/*****************************************************************************/
/******************* SPI Registers Bits **************************************/
/*****************************************************************************/
// SPI Control Register (SPICR)
#define LSBFirst            9
#define MasterTranInh       8
#define ManualSlaveAssEn    7
#define RxFifoReset         6
#define TxFifoReset         5
#define CHPA                4
#define CPOL                3
#define Master              2
#define SPE                 1
#define LOOP                0

// SPI Status Register (SPISR)
#define SlaveModeSel        5
#define MODF                4
#define TxFull              3
#define TxEmpty             2
#define RxFull              1
#define RxEmpty             0

// IP Interrupt Status Register
#define DDRNotEmpty         8
#define SlaveModeSel_int    7
#define TxFifoHalfEmpty     6
#define DDROverRun          5
#define DDRFull             4
#define DTRUnderRun         3
#define DTREmpty            2
#define SlaveMODF           1
#define MODF_int            0

/*****************************************************************************/
/************************ Functions Declarations *****************************/
/*****************************************************************************/
u32	SPI_Init(u32 axiBaseAddr, char lsbFirst, char cpha, char cpol);
u32 SPI_TransferData(u32 axiBaseAddr, char txSize, char* txBuf, char rxSize, char* rxBuf, char ssNo);

#endif /*__SPI_H__*/

