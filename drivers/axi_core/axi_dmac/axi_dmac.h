/***************************************************************************//**
 *   @file   axi_dmac.h
 *   @brief  Driver for the Analog Devices AXI-DMAC core.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef AXI_DMAC_H_
#define AXI_DMAC_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AXI_DMAC_REG_IRQ_MASK		0x80
#define AXI_DMAC_REG_IRQ_PENDING	0x84
#define AXI_DMAC_IRQ_SOT			NO_OS_BIT(0)
#define AXI_DMAC_IRQ_EOT			NO_OS_BIT(1)

#define AXI_DMAC_REG_INTF_DESC		0x010
#define AXI_DMAC_DMA_BPB_DEST		NO_OS_GENMASK(3,0)
#define AXI_DMAC_DMA_TYPE_DEST		NO_OS_GENMASK(5,4)
#define AXI_DMAC_DMA_BPB_SRC		NO_OS_GENMASK(11,8)
#define AXI_DMAC_DMA_TYPE_SRC		NO_OS_GENMASK(13,12)
//Define macro for src and dest

#define AXI_DMAC_REG_CTRL			0x400
#define AXI_DMAC_CTRL_ENABLE		NO_OS_BIT(0)
#define AXI_DMAC_CTRL_DISABLE		0u
#define AXI_DMAC_CTRL_PAUSE			NO_OS_BIT(1)

#define AXI_DMAC_REG_TRANSFER_ID		0x404
#define AXI_DMAC_REG_TRANSFER_SUBMIT	0x408
#define AXI_DMAC_TRANSFER_SUBMIT		NO_OS_BIT(0)
#define AXI_DMAC_QUEUE_FULL				NO_OS_BIT(0)
#define AXI_DMAC_REG_FLAGS				0x40c
#define AXI_DMAC_REG_DEST_ADDRESS		0x410
#define AXI_DMAC_REG_SRC_ADDRESS		0x414
#define AXI_DMAC_REG_X_LENGTH			0x418
#define AXI_DMAC_REG_Y_LENGTH			0x41c
#define AXI_DMAC_REG_DEST_STRIDE		0x420
#define AXI_DMAC_REG_SRC_STRIDE			0x424
#define AXI_DMAC_REG_TRANSFER_DONE		0x428

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
enum use_irq {
	IRQ_DISABLED = 0,
	IRQ_ENABLED = 1
};

enum dma_direction {
	INVALID_DIR = 0,
	DMA_DEV_TO_MEM = 1,
	DMA_MEM_TO_DEV = 2,
	DMA_MEM_TO_MEM = 3
};

enum dma_flags {
	DMA_CYCLIC = 1,
	DMA_LAST = 2,
	DMA_PARTIAL_REPORTING_EN = 4
};

// Could be transformed to a bool
enum cyclic_transfer {
	NO = 0,
	CYCLIC = 1
};

struct axi_dma_transfer {
	uint32_t size;
	volatile bool transfer_done;
	enum cyclic_transfer cyclic;
	uint32_t src_addr;
	uint32_t dest_addr;
};

struct axi_dmac {
	const char *name;
	uint32_t base;
	enum use_irq irq_option;
	enum dma_direction direction;
	bool hw_cyclic;
	uint32_t max_length;
	uint32_t width_dst;
	uint32_t width_src;
	volatile struct axi_dma_transfer transfer;
	//Current sub-transfer properties
	uint32_t init_addr;
	uint32_t remaining_size;
	uint32_t next_src_addr;
	uint32_t next_dest_addr;
};

struct axi_dmac_init {
	const char *name;
	uint32_t base;
	enum use_irq irq_option;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
void axi_dmac_dev_to_mem_isr(void *instance);
void axi_dmac_mem_to_dev_isr(void *instance);
void axi_dmac_mem_to_mem_isr(void *instance);
void axi_dmac_write_isr(void *instance);
int32_t axi_dmac_read(struct axi_dmac *dmac, uint32_t reg_addr,
		      uint32_t *reg_data);
int32_t axi_dmac_write(struct axi_dmac *dmac, uint32_t reg_addr,
		       uint32_t reg_data);
int32_t axi_dmac_is_transfer_ready(struct axi_dmac *dmac, bool *rdy);
int32_t axi_dmac_init(struct axi_dmac **adc_core,
		      const struct axi_dmac_init *init);
int32_t axi_dmac_remove(struct axi_dmac *dmac);
int32_t axi_dmac_transfer_start(struct axi_dmac *dmac,
				struct axi_dma_transfer *dma_transfer);
int32_t axi_dmac_transfer_wait_completion(struct axi_dmac *dmac,
		uint32_t timeout_ms);
void axi_dmac_transfer_stop(struct axi_dmac *dmac);

#endif
