/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Parameters Definitions.
 *   @author DBogdan (dragos.bogdan@analog.com)
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
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#include "hps.h"
#include "system_bd.h"

#define SPI_BASEADDR				(ALT_LWFPGASLVS_OFST + SYS_HPS_BRIDGES_SYS_SPI_BASE)
#define GPIO_BASEADDR				(ALT_LWFPGASLVS_OFST + SYS_HPS_BRIDGES_SYS_GPIO_OUT_BASE)
#define AD9361_RX_0_BASEADDR			(ALT_LWFPGASLVS_OFST + SYS_HPS_BRIDGES_AXI_AD9361_BASE)
#define AD9361_TX_0_BASEADDR			(ALT_LWFPGASLVS_OFST + SYS_HPS_BRIDGES_AXI_AD9361_BASE + 0x4000)
#define AD9361_RX_1_BASEADDR			(ALT_LWFPGASLVS_OFST + SYS_HPS_BRIDGES_AXI_AD9361_BASE)
#define AD9361_TX_1_BASEADDR			(ALT_LWFPGASLVS_OFST + SYS_HPS_BRIDGES_AXI_AD9361_BASE + 0x4000)
#define CF_AD9361_RX_DMA_BASEADDR		(ALT_LWFPGASLVS_OFST + SYS_HPS_BRIDGES_AXI_ADC_DMA_BASE)
#define CF_AD9361_TX_DMA_BASEADDR		(ALT_LWFPGASLVS_OFST + SYS_HPS_BRIDGES_AXI_DAC_DMA_BASE)
#define ADC_DDR_BASEADDR			(SYS_HPS_BRIDGES_SYS_HPS_AXI_SDRAM_BASE + 0x0800000)
#define DAC_DDR_BASEADDR			(SYS_HPS_BRIDGES_SYS_HPS_AXI_SDRAM_BASE + 0xA000000)

#define GPIO_TXNRX_PIN        			0
#define GPIO_ENABLE_PIN				1
#define GPIO_SYNC_PIN				2
#define GPIO_RESET_PIN				4
#define SPI_DEVICE_ID				0
#define GPIO_DEVICE_ID				0

#endif // __PARAMETERS_H__
