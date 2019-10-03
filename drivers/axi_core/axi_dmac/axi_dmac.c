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
#include "platform_drivers.h"
#include "util.h"
#include "axi_dmac.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AXI_DMAC_REG_IRQ_MASK		0x80
#define AXI_DMAC_REG_IRQ_PENDING	0x84
#define AXI_DMAC_IRQ_SOT			BIT(0)
#define AXI_DMAC_IRQ_EOT			BIT(1)

#define AXI_DMAC_REG_CTRL			0x400
#define AXI_DMAC_CTRL_ENABLE		BIT(0)
#define AXI_DMAC_CTRL_PAUSE			BIT(1)

#define AXI_DMAC_REG_TRANSFER_ID	0x404
#define AXI_DMAC_REG_START_TRANSFER	0x408
#define AXI_DMAC_REG_FLAGS			0x40c
#define AXI_DMAC_REG_DEST_ADDRESS	0x410
#define AXI_DMAC_REG_SRC_ADDRESS	0x414
#define AXI_DMAC_REG_X_LENGTH		0x418
#define AXI_DMAC_REG_Y_LENGTH		0x41c
#define AXI_DMAC_REG_DEST_STRIDE	0x420
#define AXI_DMAC_REG_SRC_STRIDE		0x424
#define AXI_DMAC_REG_TRANSFER_DONE	0x428

/***************************************************************************//**
 * @brief axi_dmac_read
 *******************************************************************************/
int32_t axi_dmac_read(struct axi_dmac *dmac,
		      uint32_t reg_addr,
		      uint32_t *reg_data)
{
	dmac->axi_io_read(dmac->base, reg_addr, reg_data);

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_dmac_write
 *******************************************************************************/
int32_t axi_dmac_write(struct axi_dmac *dmac,
		       uint32_t reg_addr,
		       uint32_t reg_data)
{
	dmac->axi_io_write(dmac->base, reg_addr, reg_data);

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
	} while(reg_val != (AXI_DMAC_IRQ_SOT | AXI_DMAC_IRQ_EOT));
	axi_dmac_write(dmac, AXI_DMAC_REG_IRQ_PENDING, reg_val);

	/* Wait until the transfer with the ID transfer_id is completed. */
	do {
		axi_dmac_read(dmac, AXI_DMAC_REG_TRANSFER_DONE, &reg_val);
	} while((reg_val & (1 << transfer_id)) != (1 << transfer_id));

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
	dmac->axi_io_read = init->axi_io_read;
	dmac->axi_io_write = init->axi_io_write;

	*dmac_core = dmac;

	return SUCCESS;
}
