/***************************************************************************//**
 *   @file   axi_dmac.c
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "axi_io.h"
#include "error.h"
#include "delay.h"
#include "axi_dmac.h"

/***************************************************************************//**
 * @brief dma_isr
*******************************************************************************/
void axi_dmac_default_isr(void *instance)
{
	struct axi_dmac *dmac = (struct axi_dmac *)instance;
	uint32_t remaining_size, burst_size;
	uint32_t reg_val;

	/* Get interrupt sources and clear interrupts. */
	axi_dmac_read(dmac, AXI_DMAC_REG_IRQ_PENDING, &reg_val);
	axi_dmac_write(dmac, AXI_DMAC_REG_IRQ_PENDING, reg_val);

	if ((reg_val & AXI_DMAC_IRQ_SOT) && (dmac->big_transfer.size != 0)) {
		remaining_size = dmac->big_transfer.size -
				 dmac->big_transfer.size_done;
		burst_size = (remaining_size <= dmac->transfer_max_size) ?
			     remaining_size : dmac->transfer_max_size;

		dmac->big_transfer.size_done += burst_size;
		dmac->big_transfer.address += burst_size;
		switch (dmac->direction) {
		case DMA_DEV_TO_MEM:
			axi_dmac_write(dmac, AXI_DMAC_REG_DEST_ADDRESS,
				       dmac->big_transfer.address);
			axi_dmac_write(dmac, AXI_DMAC_REG_DEST_STRIDE, 0x0);
			break;
		case DMA_MEM_TO_DEV:
			axi_dmac_write(dmac, AXI_DMAC_REG_SRC_ADDRESS,
				       dmac->big_transfer.address);
			axi_dmac_write(dmac, AXI_DMAC_REG_SRC_STRIDE, 0x0);
			break;
		default:
			return; // Other directions are not supported yet
		}
		/* The current transfer was started and a new one is queued. */
		axi_dmac_write(dmac, AXI_DMAC_REG_X_LENGTH,
			       burst_size);
		axi_dmac_write(dmac, AXI_DMAC_REG_Y_LENGTH, 0x0);

		axi_dmac_write(dmac, AXI_DMAC_REG_START_TRANSFER, 0x1);
	}
	if (reg_val & AXI_DMAC_IRQ_EOT) {
		dmac->big_transfer.transfer_done = true;
		dmac->big_transfer.address = 0;
		dmac->big_transfer.size = 0;
		dmac->big_transfer.size_done = 0;
	}
}

/***************************************************************************//**
 * @brief axi_dmac_read
 *******************************************************************************/
int32_t axi_dmac_read(struct axi_dmac *dmac,
		      uint32_t reg_addr,
		      uint32_t *reg_data)
{
	axi_io_read(dmac->base, reg_addr, reg_data);

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_dmac_write
 *******************************************************************************/
int32_t axi_dmac_write(struct axi_dmac *dmac,
		       uint32_t reg_addr,
		       uint32_t reg_data)
{
	axi_io_write(dmac->base, reg_addr, reg_data);

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_dmac_transfer_nonblock
 *******************************************************************************/
int32_t axi_dmac_transfer_nonblocking(struct axi_dmac *dmac,
				      uint32_t address, uint32_t size)
{
	uint32_t reg_val;

	if (size == 0)
		return SUCCESS; /* nothing to do */

	axi_dmac_read(dmac, AXI_DMAC_REG_CTRL, &reg_val);
	if (!(reg_val & AXI_DMAC_CTRL_ENABLE)) {
		axi_dmac_write(dmac, AXI_DMAC_REG_CTRL, 0x0);
		axi_dmac_write(dmac, AXI_DMAC_REG_CTRL, AXI_DMAC_CTRL_ENABLE);
		axi_dmac_write(dmac, AXI_DMAC_REG_IRQ_MASK, 0x0);
	}

	axi_dmac_read(dmac, AXI_DMAC_REG_START_TRANSFER, &reg_val);
	if (!(reg_val & 1)) {
		switch (dmac->direction) {
		case DMA_DEV_TO_MEM:
			axi_dmac_write(dmac, AXI_DMAC_REG_DEST_ADDRESS, address);
			axi_dmac_write(dmac, AXI_DMAC_REG_DEST_STRIDE, 0x0);
			break;
		case DMA_MEM_TO_DEV:
			axi_dmac_write(dmac, AXI_DMAC_REG_SRC_ADDRESS, address);
			axi_dmac_write(dmac, AXI_DMAC_REG_SRC_STRIDE, 0x0);
			break;
		default:
			return FAILURE; // Other directions are not supported yet
		}
		if ((size - 1) > dmac->transfer_max_size) {
			dmac->big_transfer.address = address;
			dmac->big_transfer.size = size - 1;
			dmac->big_transfer.size_done = dmac->transfer_max_size;

			axi_dmac_write(dmac, AXI_DMAC_REG_X_LENGTH,
				       dmac->transfer_max_size);
			axi_dmac_write(dmac, AXI_DMAC_REG_Y_LENGTH, 0x0);

			axi_dmac_write(dmac, AXI_DMAC_REG_FLAGS, dmac->flags);

			axi_dmac_write(dmac, AXI_DMAC_REG_START_TRANSFER, 0x1);
		} else {
			axi_dmac_write(dmac, AXI_DMAC_REG_X_LENGTH,
				       size - 1);
			axi_dmac_write(dmac, AXI_DMAC_REG_Y_LENGTH, 0x0);

			axi_dmac_write(dmac, AXI_DMAC_REG_FLAGS, dmac->flags);

			axi_dmac_write(dmac, AXI_DMAC_REG_START_TRANSFER, 0x1);
		}
	} else {
		return FAILURE;
	}

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_dmac_is_transfer_ready
 *******************************************************************************/
int32_t axi_dmac_is_transfer_ready(struct axi_dmac *dmac, bool *rdy)
{
	*rdy = dmac->big_transfer.transfer_done;

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_dmac_transfer
 *******************************************************************************/
int32_t axi_dmac_transfer(struct axi_dmac *dmac,
			  uint32_t address, uint32_t size)
{
	uint32_t transfer_id;
	uint32_t reg_val;
	uint32_t timeout = 0;

	if (size == 0)
		return SUCCESS; /* nothing to do */

	axi_dmac_write(dmac, AXI_DMAC_REG_CTRL, 0x0);
	axi_dmac_write(dmac, AXI_DMAC_REG_CTRL, AXI_DMAC_CTRL_ENABLE);

	axi_dmac_write(dmac, AXI_DMAC_REG_IRQ_MASK, 0x0);

	axi_dmac_read(dmac, AXI_DMAC_REG_TRANSFER_ID, &transfer_id);
	axi_dmac_read(dmac, AXI_DMAC_REG_IRQ_PENDING, &reg_val);
	axi_dmac_write(dmac, AXI_DMAC_REG_IRQ_PENDING, reg_val);

	switch (dmac->direction) {
	case DMA_DEV_TO_MEM:
		axi_dmac_write(dmac, AXI_DMAC_REG_DEST_ADDRESS, address);
		axi_dmac_write(dmac, AXI_DMAC_REG_DEST_STRIDE, 0x0);
		break;
	case DMA_MEM_TO_DEV:
		axi_dmac_write(dmac, AXI_DMAC_REG_SRC_ADDRESS, address);
		axi_dmac_write(dmac, AXI_DMAC_REG_SRC_STRIDE, 0x0);
		break;
	default:
		return FAILURE; // Other directions are not supported yet
	}

	if ((size - 1) > dmac->transfer_max_size) {
		dmac->big_transfer.address = address;
		dmac->big_transfer.size = size - 1;
		dmac->big_transfer.size_done = dmac->transfer_max_size;

		axi_dmac_write(dmac, AXI_DMAC_REG_X_LENGTH,
			       dmac->transfer_max_size);
		axi_dmac_write(dmac, AXI_DMAC_REG_Y_LENGTH, 0x0);

		axi_dmac_write(dmac, AXI_DMAC_REG_FLAGS, dmac->flags);

		axi_dmac_write(dmac, AXI_DMAC_REG_START_TRANSFER, 0x1);

		while(!dmac->big_transfer.transfer_done) {
			timeout++;
			if (timeout == UINT32_MAX)
				return FAILURE;
		}

		dmac->big_transfer.address = 0;
		dmac->big_transfer.size = 0;
		dmac->big_transfer.size_done = 0;

		return SUCCESS;
	}

	axi_dmac_write(dmac, AXI_DMAC_REG_X_LENGTH, size - 1);
	axi_dmac_write(dmac, AXI_DMAC_REG_Y_LENGTH, 0x0);

	axi_dmac_write(dmac, AXI_DMAC_REG_FLAGS, dmac->flags);

	axi_dmac_write(dmac, AXI_DMAC_REG_START_TRANSFER, 0x1);

	if (dmac->flags & DMA_CYCLIC)
		return SUCCESS;

	/* Wait until the new transfer is queued. */
	do {
		axi_dmac_read(dmac, AXI_DMAC_REG_START_TRANSFER, &reg_val);
	} while(reg_val == 1);

	/* Wait until the current transfer is completed. */
	do {
		axi_dmac_read(dmac, AXI_DMAC_REG_IRQ_PENDING, &reg_val);
		if (reg_val == (AXI_DMAC_IRQ_SOT | AXI_DMAC_IRQ_EOT))
			break;
	} while(!dmac->big_transfer.transfer_done);
	if (reg_val != (AXI_DMAC_IRQ_SOT | AXI_DMAC_IRQ_EOT))
		axi_dmac_write(dmac, AXI_DMAC_REG_IRQ_PENDING, reg_val);

	/* Wait until the transfer with the ID transfer_id is completed. */
	do {
		axi_dmac_read(dmac, AXI_DMAC_REG_TRANSFER_DONE, &reg_val);
	} while((reg_val & (1u << transfer_id)) != (1u << transfer_id));

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_dmac_init
 *******************************************************************************/
int32_t axi_dmac_init(struct axi_dmac **dmac_core,
		      const struct axi_dmac_init *init)
{
	struct axi_dmac *dmac;

	dmac = (struct axi_dmac *)malloc(sizeof(*dmac));
	if (!dmac)
		return FAILURE;

	dmac->name = init->name;
	dmac->base = init->base;
	dmac->direction = init->direction;
	dmac->flags = init->flags;
	dmac->transfer_max_size = -1;
	dmac->big_transfer.address = 0;
	dmac->big_transfer.size = 0;
	dmac->big_transfer.size_done = 0;

	axi_dmac_write(dmac, AXI_DMAC_REG_X_LENGTH, dmac->transfer_max_size);
	axi_dmac_read(dmac, AXI_DMAC_REG_X_LENGTH, &dmac->transfer_max_size);

	*dmac_core = dmac;

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_dmac_remove
 *******************************************************************************/
int32_t axi_dmac_remove(struct axi_dmac *dmac)
{
	if(!dmac)
		return FAILURE;

	free(dmac);

	return SUCCESS;
}
