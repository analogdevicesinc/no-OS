/***************************************************************************//**
 * @file dmac_core.h
 * @brief Implementation of DMAC Core Driver.
 * @author Istvan Csomortani (istvan.csomortani@analog.com)
 ********************************************************************************
 * Copyright 2014-2015(c) Analog Devices, Inc.
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
 *******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "dmac_core.h"

/***************************************************************************//**
 * @brief dmac_read
 *******************************************************************************/

int32_t dmac_read(dmac_core core,
		uint32_t reg_addr,
		uint32_t *reg_data)
{
	*reg_data = ad_reg_read((core.base_address + reg_addr));

	return 0;
}

/***************************************************************************//**
 * @brief dmac_write
 *******************************************************************************/

int32_t dmac_write(dmac_core core,
		uint32_t reg_addr,
		uint32_t reg_data)
{
	ad_reg_write((core.base_address + reg_addr), reg_data);

	return 0;
}

/***************************************************************************//**
 * @brief dmac_start_transaction
 *******************************************************************************/

int32_t dmac_start_transaction(dmac_core dma)
{

	uint32_t reg_val = 0;
	uint32_t timer = 0;

	dmac_write(dma, DMAC_REG_CTRL, 0x0);
	dmac_write(dma, DMAC_REG_CTRL, DMAC_CTRL_ENABLE);

	dmac_write(dma, DMAC_REG_IRQ_MASK, 0x0);

        if(dma.transfer) {
	        dmac_read(dma, DMAC_REG_TRANSFER_ID, &(dma.transfer->id));
        } else {
                ad_printf("%s : Undefined DMA transfer.\n", __func__);
                return -1;
        }
	dmac_read(dma, DMAC_REG_IRQ_PENDING, &reg_val);

	dmac_write(dma, DMAC_REG_IRQ_PENDING, reg_val);

	if(dma.type == DMAC_RX) {
		dmac_write(dma, DMAC_REG_DEST_ADDRESS, dma.transfer->start_address);
		dmac_write(dma, DMAC_REG_DEST_STRIDE, 0x0);
	} else {    /* DMAC_TX */
		dmac_write(dma, DMAC_REG_SRC_ADDRESS, dma.transfer->start_address);
		dmac_write(dma, DMAC_REG_SRC_STRIDE, 0x0);
		dmac_write(dma, DMAC_REG_FLAGS, dma.flags);
	}
	dmac_write(dma, DMAC_REG_X_LENGTH, (2 * dma.transfer->no_of_samples) - 1);
	dmac_write(dma, DMAC_REG_Y_LENGTH, 0x0);

	dmac_write(dma, DMAC_REG_START_TRANSFER, 0x1);
	if (dma.flags & DMAC_FLAGS_CYCLIC)
		return 0;
	/* Wait until the new transfer is queued. */
	do {
		dmac_read(dma, DMAC_REG_START_TRANSFER, &reg_val);
	}
	while(reg_val == 1);

	/* Wait until the current transfer is completed. */
	do {
		dmac_read(dma, DMAC_REG_IRQ_PENDING, &reg_val);
	}
	while(reg_val != (DMAC_IRQ_SOT | DMAC_IRQ_EOT));
	dmac_write(dma, DMAC_REG_IRQ_PENDING, reg_val);

	/* Wait until the transfer with the ID transfer_id is completed. */
	do {
		dmac_read(dma, DMAC_REG_TRANSFER_DONE, &reg_val);
		timer++;
		if(timer == TIMEOUT) {
			return -1;
		}
		mdelay(1);
	}
	while((reg_val & (1 << dma.transfer->id)) != (1 << dma.transfer->id));

#ifdef XILINX
	Xil_DCacheInvalidateRange(dma.transfer->start_address, (2 * dma.transfer->no_of_samples));
#endif

	return 0;
}

