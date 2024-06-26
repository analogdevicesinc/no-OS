/***************************************************************************//**
 *   @file   no_os_dma.h
 *   @brief  Platform independent function definitions and data types
 * 	     for the DMA API.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#ifndef _NO_OS_DMA_H_
#define _NO_OS_DMA_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "no_os_list.h"
#include "no_os_irq.h"
#include "no_os_mutex.h"

/**
 * @enum no_os_dma_xfer_type
 * @brief Supported transfer directions
 */
enum no_os_dma_xfer_type {
	MEM_TO_MEM,
	MEM_TO_DEV,
	DEV_TO_MEM,
};

/**
 * @struct no_os_dma_default_handler_data
 * @brief IRQ parameter for the default inter transfer handler
 */
struct no_os_dma_default_handler_data {
	struct no_os_dma_desc *desc;
	struct no_os_dma_ch *channel;
	void *extra;
};

struct no_os_dma_platform_ops;

/**
 * @struct no_os_dma_xfer_desc
 * @brief It's used to setup a generic DMA transfer.
 */
struct no_os_dma_xfer_desc {
	/** Source address for the data */
	uint8_t *src;
	/** Destination address for the data */
	uint8_t *dst;
	/** Transfer length in bytes */
	uint32_t length;
	/** Transfer direction */
	enum no_os_dma_xfer_type xfer_type;

	/** This function will be called once the transfer completes. */
	void (*xfer_complete_cb)(struct no_os_dma_xfer_desc *,
				 struct no_os_dma_xfer_desc *,
				 void *);
	/**
	 * Parameter for the transfer complete callback. This data should
	 * be valid at least until the callback is invoked.
	 */
	void *xfer_complete_ctx;
	/** Transfer complete interrupt priority level */
	uint32_t irq_priority;

	/**
	 * Peripheral which originated the transfer. Will be used to add
	 * the callback to the proper event list.
	 */
	enum no_os_irq_peripheral periph;

	/** User or platform defined data */
	void *extra;
};

/**
 * @struct no_os_dma_ch
 * @brief Describes the state of a DMA channel.
 */
struct no_os_dma_ch {
	/** Channel number */
	uint32_t id;
	/** Whether or not there is a transfer in progress on this channel */
	bool free;
	/** List of transfers for this channel */
	struct no_os_list_desc *sg_list;
	/** Channel specific interrupt line number */
	uint32_t irq_num;
	/** irq callback */
	struct no_os_callback_desc cb_desc;
	/** IRQ parameter for the default inter transfer handler */
	struct no_os_dma_default_handler_data irq_ctx;
	void *extra;

	/** Used to synchronize channel specific operations */
	void *mutex;

	/**
	 * Mark the channel as locked, in order to prevent it from being acquired
	 * even if it's free. Used as a synchronization mechanism between channels.
	 */
	bool sync_lock;
};

/**
 * @struct no_os_dma_desc
 * @brief Describes the state of the DMA controller.
 */
struct no_os_dma_desc {
	/** Unique id of a DMA controller */
	uint32_t id;
	/** The number of channels associated with the controller */
	uint32_t num_ch;
	/** References to channels associated with the controller */
	struct no_os_dma_ch *channels;
	/** Platform specific DMA functions */
	struct no_os_dma_platform_ops *platform_ops;
	/**
	 * Interrupt controller configured by the platform specific DMA functions.
	 * May be left uninitialized,Start resulting in the interrupt handler not being
	 * called once a transfer completes. As such, the user (or platform code) has
	 * to provide the cycling mechanism.
	 */
	struct no_os_irq_ctrl_desc *irq_ctrl;
	/** Platform specific data */
	void *extra;
	/** Reference counter */
	uint32_t ref;
	/** Used to synchronize DMA controller specific operations */
	void *mutex;

	/**
	 * Interrupt handler function called when a transfer is completed.
	 * It's used to configure the DMA controller for the next transfer in
	 * a channel's list.
	 */
	void (*sg_handler)(void *);
};

/**
 * @struct no_os_dma_init_param
 * @brief Initialization parameter for the DMA controller
 */
struct no_os_dma_init_param {
	/** Unique id of a DMA controller */
	uint32_t id;
	/** The number of channels associated with the controller */
	uint32_t num_ch;
	/** Platform specific DMA functions */
	struct no_os_dma_platform_ops *platform_ops;
	/** Platform specific data */
	void *extra;

	/**
	 * Optional custom scatter gather callback. May be set to NULL in
	 * order to use the default one.
	 */
	void (*sg_handler)(void *);
	/**
	 * Optional custom parameter for the scatter gather callback. Only takes
	 * effect if the handler is also provided.
	 */
	void *ctx;
};

/** Initialize a DMA controller. */
int no_os_dma_init(struct no_os_dma_desc **,
		   struct no_os_dma_init_param *);

/** Free the resources allocated by the DMA layer. Disables the DMA controller. */
int no_os_dma_remove(struct no_os_dma_desc *);

/** Acquire a channel and add the transfers to it's SG list. */
int no_os_dma_config_xfer(struct no_os_dma_desc *,
			  struct no_os_dma_xfer_desc *,
			  uint32_t,
			  struct no_os_dma_ch *);

/** Prevent a channel from being acquired, even if it's free. */
int no_os_dma_chan_lock(struct no_os_dma_ch *);

/** Allow a channel to be acquired, in case it's free. */
int no_os_dma_chan_unlock(struct no_os_dma_ch *);

/** Start the DMA transactions for a specific channel. */
int no_os_dma_xfer_start(struct no_os_dma_desc *, struct no_os_dma_ch *);

/**
 * Stop the DMA transactions for a specific channel and remove the transfers
 * from the SG list.
 */
int no_os_dma_xfer_abort(struct no_os_dma_desc *, struct no_os_dma_ch *);

/**
 * Check whether a transfer is complete. By default, it requires the interrupt
 * handler to set the free flag for the channel.
 */
bool no_os_dma_is_completed(struct no_os_dma_desc *, struct no_os_dma_ch *);

/** Whether or not a specific channel has an ongoing transfer. */
bool no_os_dma_in_progress(struct no_os_dma_desc *, struct no_os_dma_ch *);

/** Get a free DMA channel. */
int no_os_dma_acquire_channel(struct no_os_dma_desc *, struct no_os_dma_ch **);

/** Free a DMA channel. */
int no_os_dma_release_channel(struct no_os_dma_desc *, struct no_os_dma_ch *);

/**
 * @struct no_os_irq_platform_ops
 * @brief Structure holding IRQ function pointers that point to the platform
 * specific function
 */
struct no_os_dma_platform_ops {
	/** Initialize platform specific resources for the DMA controller. */
	int (*dma_init)(struct no_os_dma_desc **, struct no_os_dma_init_param *);
	/** Remove resources allocated for the DMA controller. */
	int (*dma_remove)(struct no_os_dma_desc *desc);
	/** Get a free channel and set it as busy. */
	int (*dma_acquire_ch)(struct no_os_dma_desc *, uint32_t *);
	int (*dma_release_ch)(struct no_os_dma_desc *, uint32_t);
	/** Configure platform specific settings required for the current DMA transfer */
	int (*dma_config_xfer)(struct no_os_dma_ch *,
			       struct no_os_dma_xfer_desc *);
	/** Signal the DMA controller to start the transfer. */
	int (*dma_xfer_start)(struct no_os_dma_desc *, struct no_os_dma_ch *);
	/** Signal the DMA controller to stop the transfer. */
	int (*dma_xfer_abort)(struct no_os_dma_desc *, struct no_os_dma_ch *);
	/** Whether or not a specific channel has an ongoing or future transfer configured */
	bool (*dma_ch_is_completed)(struct no_os_dma_desc *, struct no_os_dma_ch *);
	/**
	 * Whether or not a specific channel has an ongoing transfer.
	 * This returns true between transfers.
	 */
	bool (*dma_ch_in_progress)(struct no_os_dma_desc *, struct no_os_dma_ch *);
};

#endif
