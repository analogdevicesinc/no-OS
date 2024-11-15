/***************************************************************************//**
 *   @file   xdma_config.h
 *   @brief  Xilinx AXI DMA configuration file.
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
