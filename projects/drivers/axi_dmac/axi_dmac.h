/***************************************************************************//**
 *   @file   axi_dmac.h
 *   @brief  Driver for the Analog Devices AXI-DMAC core.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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
#ifndef AXI_DMAC_H_
#define AXI_DMAC_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
enum dma_direction {
	DMA_DEV_TO_MEM,
	DMA_MEM_TO_DEV
};

enum dma_flags {
	DMA_CYCLIC = 1,
	DMA_LAST = 2
};

struct axi_dmac {
	const char *name;
	uint32_t base;
	enum dma_direction direction;
	enum dma_flags flags;
	uint32_t (*dmac_read)(uint32_t, uint32_t);
	void (*dmac_write)(uint32_t, uint32_t, uint32_t);
};

struct axi_dmac_init {
	const char *name;
	uint32_t base;
	enum dma_direction direction;
	enum dma_flags flags;
	uint32_t (*dmac_read)(uint32_t, uint32_t);
	void (*dmac_write)(uint32_t, uint32_t, uint32_t);
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t axi_dmac_transfer(struct axi_dmac *dmac,
			  uint32_t address, uint32_t size);
int32_t axi_dmac_init(struct axi_dmac **adc_core,
		      const struct axi_dmac_init *init);

#endif
