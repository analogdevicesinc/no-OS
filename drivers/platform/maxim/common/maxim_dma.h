/***************************************************************************//**
 *   @file   maxim_dma.h
 *   @brief  Maxim platform specific header DMA API.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
