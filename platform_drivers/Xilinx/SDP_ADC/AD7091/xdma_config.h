/***************************************************************************//**
 *   @file   xdma_config.h
 *   @brief  Xilinx AXI DMA configuration file.
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
