/*******************************************************************************
 *   @file   maxim_capi_dma.h
 *   @brief  Header file for DMA functions with CAPI
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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

#ifndef MAXIM_CAPI_DMA_H_
#define MAXIM_CAPI_DMA_H_

#include "dma.h"
#include "capi_dma.h"

#if !defined(CONFIG_TRUSTED_EXECUTION_SECURE) || (CONFIG_TRUSTED_EXECUTION_SECURE != 1)
#error "CONFIG_TRUSTED_EXECUTION_SECURE must be defined and set to 1."
#endif

enum max_capi_dma_request {
	MAX_CAPI_DMA_REQUEST_MEMTOMEM = MXC_DMA_REQUEST_MEMTOMEM,
	MAX_CAPI_DMA_REQUEST_SPI_RX = MXC_DMA_REQUEST_SPIRX,
	MAX_CAPI_DMA_REQUEST_SPI_TX = MXC_DMA_REQUEST_SPITX,
	MAX_CAPI_DMA_REQUEST_UART_RX = MXC_DMA_REQUEST_UARTRX,
	MAX_CAPI_DMA_REQUEST_UART_TX = MXC_DMA_REQUEST_UARTTX,
	MAX_CAPI_DMA_REQUEST_I3C_RX_CONTROLLER = MXC_DMA_REQUEST_I3CRX_CONT,
	MAX_CAPI_DMA_REQUEST_I3C_TX_CONTROLLER = MXC_DMA_REQUEST_I3CTX_CONT,
	MAX_CAPI_DMA_REQUEST_I3C_RX_TARGET = MXC_DMA_REQUEST_I3CRX_TARG,
	MAX_CAPI_DMA_REQUEST_I3C_TX_TARGET = MXC_DMA_REQUEST_I3CTX_TARG,
	MAX_CAPI_DMA_REQUEST_AES_RX = MXC_DMA_REQUEST_AESRX,
	MAX_CAPI_DMA_REQUEST_AES_TX = MXC_DMA_REQUEST_AESTX,
	MAX_CAPI_DMA_REQUEST_CRC_TX = MXC_DMA_REQUEST_CRCTX,
};

struct max_capi_dma_priv {
	/** Identifier */
	uint64_t id;
	/** Array of pointers to channels */
	struct capi_dma_chan **channels;
	/** Number of channels storage */
	uint8_t num_channels;
};

struct max_capi_dma_ch_priv {
	/* MXC_DMA_AcquireChannel returns a channel ID and is stored here */
	uint32_t hw_channel_id;
};

struct max_capi_dma_xfer_extra {
	/** DMA request type */
	enum max_capi_dma_request reqsel;
};

extern const struct capi_dma_ops max_capi_dma_ops;

#endif /* MAXIM_CAPI_DMA_H_ */
