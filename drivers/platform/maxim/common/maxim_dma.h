/***************************************************************************//**
 *   @file   maxim_dma.h
 *   @brief  Maxim platform specific header DMA API.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef MAXIM_DMA_H_
#define MAXIM_DMA_H_

#include <stdint.h>
#include "no_os_dma.h"
#include "dma.h"

#if TARGET_NUM == 32650
#include "max32650.h"
#elif TARGET_NUM == 32655
#include "max32655.h"
#elif TARGET_NUM == 32660
#include "max32660.h"
#elif TARGET_NUM == 32665
#include "max32665.h"
#elif TARGET_NUM == 32690
#include "max32690.h"
#elif TARGET_NUM == 78000
#include "max78000.h"
#endif

#define MAX_DMA			((struct max_dma_regs *)(MXC_DMA))
#define MAX_DMA_IRQ_EN_ALL_CH	NO_OS_GENMASK(15, 0)
#define MAX_DMA_ENABLED		NO_OS_BIT(0)
#define MAX_DMA_ENABLE		NO_OS_BIT(0)
#define MAX_DMA_IRQ_CTZ_EN	NO_OS_BIT(31)
#define MAX_DMA_REQSEL_MASK	NO_OS_GENMASK(9, 4)
#define MAX_DMA_DST_INC		NO_OS_BIT(22)
#define MAX_DMA_SRC_INC		NO_OS_BIT(18)

struct max_dma_ch_regs {
	volatile uint32_t cfg;
	volatile uint32_t st;
	volatile uint32_t src;
	volatile uint32_t dst;
	volatile uint32_t cnt;
	volatile uint32_t src_rld;
	volatile uint32_t dst_rld;
	volatile uint32_t cnt_rld;
};

struct max_dma_regs {
	volatile uint32_t cn;
	volatile const uint32_t intr;
	const uint32_t rsv_0x8_0xff[62];
	volatile struct max_dma_ch_regs ch[MXC_DMA_CHANNELS];
};

extern struct no_os_dma_platform_ops max_dma_ops;

/**
 * @brief Solve a naming compatibility issue between the MAX32665
 * 	  and the rest of the Maxim MCUs.
 * @param id - DMA controller device id.
 * @param ch - DMA channel number.
 * @return 0 in case of success
 * 	   -ENOMEM if there is not enough free memory
 */
static inline uint32_t max_dma_get_irq(uint32_t id, uint32_t ch)
{
#if TARGET_NUM == 32665
	if (!id)
		return MXC_DMA0_CH_GET_IRQ(ch);

	return MXC_DMA1_CH_GET_IRQ(ch);
#else
	return MXC_DMA_CH_GET_IRQ(ch);
#endif
}

#endif
