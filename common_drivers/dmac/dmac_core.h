/***************************************************************************//**
 * @file dmac_core.h
 * @brief Header file of DMAC Core Driver.
 * @author Istvan Csomortani (istvan.csomortani@analog.com)
 ********************************************************************************
 * Copyright 2014-2015(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * - Neither the name of Analog Devices, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * - The use of this software may or may not infringe the patent rights
 * of one or more patent holders. This license does not release you
 * from the requirement that you obtain separate licenses from these
 * patent holders to use this software.
 * - Use of the software either in source or binary form, must be run
 * on or directly connected to an Analog Devices Inc. component.
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
#ifndef DMAC_CORE_H
#define DMAC_CORE_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "altxil_base.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define DMAC_REG_IRQ_MASK		0x80
#define DMAC_REG_IRQ_PENDING		0x84
#define DMAC_REG_IRQ_SOURCE		0x88

#define DMAC_REG_CTRL			0x400
#define DMAC_REG_TRANSFER_ID		0x404
#define DMAC_REG_START_TRANSFER		0x408
#define DMAC_REG_FLAGS			0x40c
#define DMAC_REG_DEST_ADDRESS		0x410
#define DMAC_REG_SRC_ADDRESS		0x414
#define DMAC_REG_X_LENGTH		0x418
#define DMAC_REG_Y_LENGTH		0x41c
#define DMAC_REG_DEST_STRIDE		0x420
#define DMAC_REG_SRC_STRIDE		0x424
#define DMAC_REG_TRANSFER_DONE		0x428
#define DMAC_REG_ACTIVE_TRANSFER_ID	0x42c
#define DMAC_REG_STATUS			0x430
#define DMAC_REG_CURRENT_DEST_ADDR	0x434
#define DMAC_REG_CURRENT_SRC_ADDR	0x438
#define DMAC_REG_DBG0			0x43c
#define DMAC_REG_DBG1			0x440

#define DMAC_CTRL_ENABLE		(1 << 0)
#define DMAC_CTRL_PAUSE			(1 << 1)

#define DMAC_IRQ_SOT			(1 << 0)
#define DMAC_IRQ_EOT			(1 << 1)

#define	DMAC_DOWNSTREAM			0
#define DMAC_UPSTREAM			1

#define TIMEOUT				10000

typedef struct {
	uint32_t	id;
	uint32_t	startaddr;
	uint32_t	size;
} dmac_xfer;

typedef struct {
	uint32_t	baseaddr;
	uint8_t		type;
	uint32_t	irq_id;
	dmac_xfer	transfer;
} dmac_core;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t dmac_read(dmac_core core,
		uint32_t reg_addr,
		uint32_t *reg_data);
int32_t dmac_write(dmac_core core,
		uint32_t reg_addr,
		uint32_t reg_data);

int32_t dmac_start_transaction(dmac_core core, dmac_xfer *transaction);

#endif
