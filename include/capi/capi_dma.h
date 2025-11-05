/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common HAL DMA
 */

#ifndef _CAPI_DMA_H_
#define _CAPI_DMA_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @enum capi_dma_xfer_type
 * @brief Supported transfer directions
 */
enum capi_dma_xfer_type {
	CAPI_DMA_MEM_TO_MEM,
	CAPI_DMA_MEM_TO_DEV,
	CAPI_DMA_DEV_TO_MEM,
};

/**
 * @enum capi_dma_addr_inc
 * @brief DMA address increment options.
 */
enum capi_dma_addr_inc {
	CAPI_DMA_BYTE_INCREMENT, /**< Increment address by byte */
	CAPI_DMA_NO_INCREMENT    /**< No address increment */
};

// Forward declaration of structures
struct capi_dma_ops;
struct capi_irq_handle;

/**
 * @typedef capi_dma_cpu_addr_t
 * @brief CPU physical address type for DMA operations.
 */
typedef uintptr_t capi_dma_cpu_addr_t;

/**
 * @struct capi_dma_transfer
 * @brief Represents a generic DMA transfer configuration.
 */
struct capi_dma_transfer {
	/** Source address of the data */
	capi_dma_cpu_addr_t src;
	/** Destination address of the data */
	capi_dma_cpu_addr_t dst;
	/** Length of the transfer in bytes */
	size_t length;
	/** Direction of the transfer */
	enum capi_dma_xfer_type xfer_type;
	/** Source address increment mode for each transfer. */
	enum capi_dma_addr_inc src_inc;
	/** Destination address increment mode for each transfer. */
	enum capi_dma_addr_inc dst_inc;
	/** Callback function invoked when the transfer completes */
	void (*xfer_complete_cb)(struct capi_dma_transfer *, void *xfer_complete_ctx);
	/** Interrupt priority for transfer completion */
	uint32_t irq_priority;
	/** Optional user-defined data */
	void *user_data;
	/** Optional platform-specific data. Set to NULL if unused */
	void *extra;
};

/**
 * @struct capi_dma_chan
 * @brief Represents the state and configuration of a DMA channel.
 */
struct capi_dma_chan {
	/** Indicates whether this structure is owned by the application or the
	 * driver. Set by capi_dma_init_chan(); used by capi_dma_deinit_chan(). */
	bool owned_by_app;
	/** Pointer to the associated DMA controller handle. Set by
	 * capi_dma_init_chan(). */
	struct capi_dma_handle *handle;
	/** Channel ID. Set by capi_dma_init_chan(). */
	uint32_t id;
	/** Interrupt number for this channel. Set by capi_dma_init_chan(). */
	uint32_t irq_num;
	/** Callback function invoked on transfer completion. Set by
	 * capi_dma_config_xfer(). */
	void (*xfer_complete_cb)(struct capi_dma_transfer *, void *xfer_complete_ctx);
	/** Pointer to the capi_dma_transfer object associated with the ongoing
	 * transfer. This is also passed as the context to the transfer completion
	 * callback. Set by capi_dma_config_xfer().
	 */
	struct capi_dma_transfer *xfer;
	/** Optional user-defined data */
	void *user_data;
	/** Optional platform-specific data. Set by capi_dma_init_chan(). Set to NULL
	 * if unused. */
	void *extra;
};

/**
 * @struct capi_dma_handle
 * @brief Represents a DMA controller handle.
 */
struct capi_dma_handle {
	/** Indicates whether this structure is owned by the application or the
	 * driver. Set by capi_dma_init(); used by capi_dma_deinit(). */
	bool owned_by_app;
	/** If true, the driver is owner of handle memory. */
	bool init_allocated;
	/** Pointer to function implementations. Set by capi_dma_init(); used by the
	 * CAPI thin layer. */
	const struct capi_dma_ops *ops;
	/** Optional lock used by the CAPI thin layer if supported */
	void *lock;
	/** Optional user-defined data */
	void *user_data;
	/** Optional platform-specific data. Set by capi_dma_init(). Set to NULL if
	 * unused. */
	void *extra;
};

/**
 * @struct capi_dma_config
 * @brief Configuration structure for a DMA controller.
 */
struct capi_dma_config {
	/** Unique identifier for the DMA controller */
	uint64_t id;
	/** Number of DMA channels available in the controller */
	uint32_t num_chans;
	/** Pointer to platform-specific DMA function implementations */
	const struct capi_dma_ops *ops;
	/** Offset between CPU physical and DMA-visible global address
	 *  (CPU physical address - DMA-visible address) */
	ptrdiff_t addr_offset;
	/** Handle of IRQ driver. Set to NULL if unused */
	struct capi_irq_handle *irq_handle;
	/** Optional platform-specific data. Set to NULL if unused */
	void *extra;
};

/**
 * @brief Initializes an instance of the DMA controller.
 *
 * @param [in,out] handle Points to a pointer to the DMA controller context
 * structure. If set to NULL, the driver allocates the context and takes
 * ownership. If not NULL, the application provides the structure and retains
 * ownership.
 * @param [in] config     Points to the configuration for the DMA controller.
 *
 * @return 0 on success, or an error code.
 */
int capi_dma_init(struct capi_dma_handle **handle, const struct capi_dma_config *config);

/**
 * @brief Frees the resources allocated by the DMA layer and disables the DMA
 * controller.
 *
 * @param [in] handle Points to the DMA controller context.
 *
 * @return 0 on success, or an error code.
 */
int capi_dma_deinit(struct capi_dma_handle *handle);

/**
 * @brief Initializes a DMA channel.
 *
 * @param [in]  handle Points to the DMA controller context.
 * @param [in,out] chan_ptr Points to a pointer to the DMA channel structure.
 *                        If set to NULL, the driver allocates the structure and
 * takes ownership. If not NULL, the application provides the structure and
 * retains ownership.
 * @param [in]  id     Identifier for the DMA channel to initialize.
 *
 * @return 0 on success, or an error code.
 */
int capi_dma_init_chan(struct capi_dma_handle *handle, struct capi_dma_chan **chan_ptr,
		       uint32_t id);

/**
 * @brief Deinitializes a DMA channel and releases associated resources.
 *
 * @param [in] chan Points to the DMA channel to be deinitialized.
 *
 * @return 0 on success, or an error code.
 */
int capi_dma_deinit_chan(struct capi_dma_chan *chan);

/**
 * @brief Configures platform-specific settings for a DMA transfer.
 *
 * @param [in] chan Points to the DMA channel to be configured.
 * @param [in] xfer Points to the DMA transfer configuration. This should be valid until the
 * transfer is completed.
 *
 * @return 0 on success, or an error code.
 */
int capi_dma_config_xfer(struct capi_dma_chan *chan, struct capi_dma_transfer *xfer);

/**
 * @brief Starts a DMA transfer on the specified channel.
 *
 * @param [in] chan Points to the DMA channel on which the transfer will start.
 *
 * @return 0 on success, or an error code.
 */
int capi_dma_xfer_start(struct capi_dma_chan *chan);

/**
 * @brief Aborts an ongoing DMA transfer and removes pending transfers from the
 * SG list.
 *
 * @param [in] chan Points to the DMA channel whose transfer should be aborted.
 *
 * @return 0 on success, or an error code.
 */
int capi_dma_xfer_abort(struct capi_dma_chan *chan);

/**
 * @brief Checks whether the DMA channel has completed its transfer.
 *
 * @param [in] chan Points to the DMA channel to check.
 *
 * @return true if the transfer is complete; false otherwise.
 */
bool capi_dma_chan_is_completed(const struct capi_dma_chan *chan);

/**
 * @brief DMA Driver Interrupt handler. If interrupt vectors are managed and
 * implemented by user, then user shall call this function in the relevant
 * interrupt vector function.
 *
 * @param [in] handle Points to the DMA controller context
 */
void capi_dma_isr(struct capi_dma_handle *handle);

/**
 * @brief DMA Channel Interrupt handler. If interrupt vectors are managed and
 * implemented by user, then user shall call this function in the relevant
 * interrupt vector function.
 *
 * @param [in] chan Points to the DMA channel.
 */
void capi_dma_isr_chan(struct capi_dma_chan *chan);

/**
 * @struct capi_dma_ops
 * @brief Structure holding function pointers to platform-specific DMA
 * operations.
 *
 * These are used internally by the DMA thin layer. The public API (above) wraps
 * these operations to provide a consistent interface.
 */
struct capi_dma_ops {
	/** See capi_dma_init() */
	int (*init)(struct capi_dma_handle **, const struct capi_dma_config *);
	/** See capi_dma_deinit() */
	int (*deinit)(struct capi_dma_handle *);
	/** See capi_dma_init_chan() */
	int (*init_chan)(struct capi_dma_handle *, struct capi_dma_chan **, uint32_t);
	/** See capi_dma_deinit_chan() */
	int (*deinit_chan)(struct capi_dma_chan *);
	/** See capi_dma_config_xfer() */
	int (*config_xfer)(struct capi_dma_chan *, struct capi_dma_transfer *);
	/** See capi_dma_xfer_start() */
	int (*xfer_start)(struct capi_dma_chan *);
	/** See capi_dma_xfer_abort() */
	int (*xfer_abort)(struct capi_dma_chan *);
	/** See capi_dma_chan_is_completed() */
	bool (*chan_is_completed)(const struct capi_dma_chan *);
	/** See capi_dma_isr() */
	int (*isr)(struct capi_dma_handle *);
	/** See capi_dma_isr_chan() */
	int (*isr_chan)(struct capi_dma_chan *);
};

#if defined(__cplusplus)
}
#endif

#endif
