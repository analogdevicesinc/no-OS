/***************************************************************************//**
 *   @file   xdma_config.h
 *   @brief  Xilinx AXI DMA configuration file.
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
******************************************************************************/
#ifndef __XDMA_CONFIG_H__
#define __XDMA_CONFIG_H__

XAxiDma_Config XAxiDma_ConfigTable[] = {
    {
        XPAR_AXI_DMA_0_DEVICE_ID,
        XPAR_AXI_DMA_0_BASEADDR,
        XPAR_AXI_DMA_0_SG_INCLUDE_STSCNTRL_STRM,
        XPAR_AXI_DMA_0_INCLUDE_MM2S,
        XPAR_AXI_DMA_0_INCLUDE_MM2S_DRE,
        XPAR_AXI_DMA_0_M_AXI_MM2S_DATA_WIDTH,
        XPAR_AXI_DMA_0_INCLUDE_S2MM,
        XPAR_AXI_DMA_0_INCLUDE_S2MM_DRE,
        XPAR_AXI_DMA_0_M_AXI_S2MM_DATA_WIDTH,
        XPAR_AXI_DMA_0_INCLUDE_SG,
        XPAR_AXI_DMA_0_NUM_MM2S_CHANNELS,
        XPAR_AXI_DMA_0_NUM_S2MM_CHANNELS
    }
};

#endif /* __XDMA_CONFIG_H__ */
