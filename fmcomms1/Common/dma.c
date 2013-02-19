/**************************************************************************//**
*   @file   dma.c
*   @brief  Implementation of driver file for AXI DMA CORE.
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

#include <xparameters.h>
#include <xil_io.h>
#include "dma.h"

/* Functions used with C_INCLUDE_SG = 1 */

void DMA_SG_TransferDataTX(stTransferDescriptor* pCurrentDescriptor, stTransferDescriptor* pTailDescriptor, u32 baseAddr)
{
	//stop DMA
	Xil_Out32((baseAddr + DMA_SG_MM2S_DMACR), 0x00);
	//set the current descriptor pointer
	Xil_Out32((baseAddr + DMA_SG_MM2S_CURDESC), (u32)pCurrentDescriptor);
    //start DMA
	Xil_Out32((baseAddr + DMA_SG_MM2S_DMACR), (1 << DMA_RS));
	//set the tail descriptor pointer
	Xil_Out32((baseAddr + DMA_SG_MM2S_TAILDESC), (u32)pTailDescriptor);

}

u32 DMA_SG_IsTransferCompleteTX(u32 baseAddr)
{
	return Xil_In32((baseAddr + DMA_SG_MM2S_DMASR)) & (1 << DMA_Idle);
}

void DMA_SG_TransferDataRX(stTransferDescriptor* pCurrentDescriptor, stTransferDescriptor* pTailDescriptor, u32 baseAddr)
{
	//stop DMA
	Xil_Out32((baseAddr + DMA_SG_S2MM_DMACR), 0x00);
	//set the current descriptor pointer
	Xil_Out32((baseAddr + DMA_SG_S2MM_CURDESC), (u32)pCurrentDescriptor);
    //start DMA
	Xil_Out32((baseAddr + DMA_SG_S2MM_DMACR), (1 << DMA_RS));
	//set the tail descriptor pointer
	Xil_Out32((baseAddr + DMA_SG_S2MM_TAILDESC), (u32)pTailDescriptor);

}

u32 DMA_SG_IsTransferCompleteRX(u32 baseAddr)
{
	return Xil_In32((baseAddr + DMA_S2MM_DMASR)) & (1 << DMA_Idle);
}

/* Functions used with C_INCLUDE_SG = 0 */

void DMA_TransferDataRx(u32 destAddress, u32 nrBytes, u32 baseAddr)
{
	// Stop DMA
	Xil_Out32((baseAddr + DMA_S2MM_DMACR), 0);
	// Enable DMA
	Xil_Out32((baseAddr + DMA_S2MM_DMACR), (1 << DMA_RS));
	// Destination Address
	Xil_Out32((baseAddr + DMA_S2MM_DA), destAddress);
	// Number of bytes
	Xil_Out32((baseAddr + DMA_S2MM_LENGTH), (nrBytes * 8));
}

void DMA_TransferDataTx(u32 sourceAddress, u32 nrBytes, u32 baseAddr)
{
	// Stop DMA
	Xil_Out32((baseAddr + DMA_MM2S_DMACR), 0);
	// Enable DMA
	Xil_Out32((baseAddr + DMA_MM2S_DMACR), (1 << DMA_RS));
	// Source Address
	Xil_Out32((baseAddr + DMA_MM2S_SA), sourceAddress);
	// Number of bytes
	Xil_Out32((baseAddr + DMA_MM2S_LENGTH), (nrBytes * 8));
}
