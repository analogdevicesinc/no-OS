/**************************************************************************//**
*   @file   dma.h
*   @brief  Header file for AXI DMA Core Driver.
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
*   SVN Revision: 468
******************************************************************************/

#ifndef __DMA_H__
#define __DMA_H__

/* Register Address for C_INCLUDE_SG = 1 */
#define DMA_SG_MM2S_DMACR		0x00
#define DMA_SG_MM2S_DMASR		0x04
#define DMA_SG_MM2S_CURDESC		0x08
#define DMA_SG_MM2S_TAILDESC	0x10
#define DMA_SG_S2MM_DMACR		0x30
#define DMA_SG_S2MM_DMASR		0x34
#define DMA_SG_S2MM_CURDESC		0x38
#define DMA_SG_S2MM_TAILDESC	0x40

/* Register Address for C_INCLUDE_SG = 0 */
#define DMA_MM2S_DMACR			0x00
#define DMA_MM2S_DMASR			0x04
#define DMA_MM2S_SA				0x18
#define DMA_MM2S_LENGTH			0x28
#define DMA_S2MM_DMACR			0x30
#define DMA_S2MM_DMASR			0x34
#define DMA_S2MM_DA				0x48
#define DMA_S2MM_LENGTH			0x58

/* Register specific bits */
// MM2S, S2MM DMA Control Register
#define DMA_RS					0
#define DMA_Reset				2
#define DMA_IOC_IrqEn			12
#define DMA_Dly_IrqEn			13
#define DMA_ERR_IrqEn			14
#define DMA_IRQThreshold		16
#define DMA_IRQDelay			24
// MM2S, S2MM DMA Status Register
#define DMA_Halted				0
#define DMA_Idle				1
#define DMA_SGIncld				3
#define DMA_DMAIntErr			4
#define DMA_DMASlvErr			5
#define DMA_DMADecErr			6
#define DMA_SGIntErr			8
#define DMA_SGSlvErr			9
#define DMA_SGDecErr			10
#define DMA_IOC_Irq				12
#define DMA_Dly_Irq				13
#define DMA_Err_Irq				14
#define DMA_IRQThresholdSts		16
#define DMA_IRQDlySts			24

typedef struct _stTransferDescriptor
{
    u32 NXTDESC_PNTR;
    u32 RESERVED1;
    u32 BUFFER_ADDR;
    u32 RESERVED2;
    u32 RESERVED3;
    u32 RESERVED4;
    u32 CONTROL;
    u32 STATUS;
    u32 APP0;
    u32 APP1;
    u32 APP2;
    u32 APP3;
    u32 APP4;
}stTransferDescriptor;

void 	DMA_SG_TransferDataTX(stTransferDescriptor* pCurrentDescriptor, stTransferDescriptor* pTailDescriptor, u32 baseAddr);
u32 	DMA_SG_IsTransferCompleteTX(u32 baseAddr);
void 	DMA_SG_TransferDataRX(stTransferDescriptor* pCurrentDescriptor, stTransferDescriptor* pTailDescriptor, u32 baseAddr);
u32 	DMA_SG_IsTransferCompleteRX(u32 baseAddr);
void 	DMA_TransferDataRx(u32 destAddress, u32 nrBytes, u32 baseAddr);
void 	DMA_TransferDataTx(u32 sourceAddress, u32 nrBytes, u32 baseAddr);

#endif //__DMA_H__
