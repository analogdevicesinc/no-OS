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
	CAPI_DMA_BYTE_INCREMENT,     /**< Increment address by byte */
	CAPI_DMA_HALFWORD_INCREMENT, /**< Increment address by halfword */
	CAPI_DMA_WORD_INCREMENT,     /**< Increment address by word */
	CAPI_DMA_NO_INCREMENT        /**< No address increment */
};

/**
 * @enum capi_dma_xfer_size
 * @brief DMA transfer size options.
 */
enum capi_dma_xfer_size {
	CAPI_DMA_XFER_SIZE_1_BYTE = 0x0U,   /**< 1 Byte */
	CAPI_DMA_XFER_SIZE_2_BYTES = 0x1U,  /**< 2 Bytes */
	CAPI_DMA_XFER_SIZE_4_BYTES = 0x2U,  /**< 4 Bytes */
	CAPI_DMA_XFER_SIZE_8_BYTES = 0x3U,  /**< 8 Bytes */
	CAPI_DMA_XFER_SIZE_16_BYTES = 0x4U, /**< 16 Bytes */
	CAPI_DMA_XFER_SIZE_32_BYTES = 0x5U  /**< 32 Bytes */
};

/**
 * @enum capi_dma_sg_mode
 * @brief Scatter-gather descriptor organization modes
 */
enum capi_dma_sg_mode {
	CAPI_DMA_SG_MODE_ARRAY, /**< Array mode: contiguous descriptor array (DMA, DDE array) */
	CAPI_DMA_SG_MODE_LIST,  /**< List mode: linked list descriptors (DDE list only) */
};

// Forward declaration of structures
struct capi_dma_ops;
struct capi_irq_handle;

/**
 * @typedef capi_dma_glbl_addr_t
 * @brief Global address type for DMA operations.
 */
typedef uintptr_t capi_dma_glbl_addr_t;

/**
 * @typedef capi_dma_xfer_complete_cb
 * @brief Callback function type for DMA transfer completion.
 * @param event Event type indicating the completion status
 * @param xfer_complete_ctx Context data passed to the callback
 */
typedef void (*capi_dma_xfer_complete_cb)(uint32_t event,
		void *xfer_complete_ctx);

/**
 * @typedef capi_dma_error_cb
 * @brief Callback function type for DMA error events.
 * @param event Event type indicating the error status
 * @param error_ctx Context data passed to the callback
 */
typedef void (*capi_dma_error_cb)(uint32_t event, void *error_ctx);

/**
 * @struct capi_dma_transfer
 * @brief Represents a generic DMA transfer configuration.
 */
struct capi_dma_transfer {
	/** Source address of the data */
	capi_dma_glbl_addr_t src;
	/** Destination address of the data */
	capi_dma_glbl_addr_t dst;
	/** Length of the transfer in bytes */
	size_t length;
	/** Direction of the transfer */
	enum capi_dma_xfer_type xfer_type;
	/** Source address increment mode for each transfer. */
	enum capi_dma_addr_inc src_inc;
	/** Destination address increment mode for each transfer. */
	enum capi_dma_addr_inc dst_inc;
	/** Source data size for each transfer. */
	enum capi_dma_xfer_size src_size;
	/** Destination data size for each transfer. */
	enum capi_dma_xfer_size dst_size;
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
	capi_dma_xfer_complete_cb xfer_complete_cb;
	/** Context data for transfer completion callback */
	void *xfer_complete_ctx;
	/** Callback function invoked on DMA errors. Set by
	 * capi_dma_register_error_callback(). */
	capi_dma_error_cb error_cb;
	/** Context data for error callback */
	void *error_ctx;
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
 * @brief DMA handle type
 */
struct capi_dma_handle {
	const struct capi_dma_ops
		*ops; /**< Function pointers that implement the DMA CAPI */
	bool init_allocated;            /**< If true, the driver is owner of handle memory. */
	void *priv;                     /**< Pointer to driver-specific handle extension. */
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
	/** Handle of IRQ driver. Set to NULL if unused */
	struct capi_irq_handle *irq_handle;
	/** Optional platform-specific data. Set to NULL if unused */
	void *extra;
};

/**
 * @struct capi_dma_sg_config
 * @brief Scatter-gather transfer configuration
 *
 * This structure supports both array and list scatter-gather modes.
 * - For array mode: blocks must be a contiguous array
 * - For list mode: each block's extra field can contain next-descriptor pointer
 */
struct capi_dma_sg_config {
	/** Number of blocks in the array/list */
	uint32_t num_blocks;
	/** Scatter-gather mode */
	enum capi_dma_sg_mode mode;
	/** Direction of the transfer (applies to all blocks) */
	enum capi_dma_xfer_type xfer_type;
	/** Optional user-defined data */
	void *user_data;
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
int capi_dma_init(struct capi_dma_handle **handle,
		  const struct capi_dma_config *config);

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
int capi_dma_init_chan(struct capi_dma_handle *handle,
		       struct capi_dma_chan **chan_ptr,
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
int capi_dma_config_xfer(struct capi_dma_chan *chan,
			 struct capi_dma_transfer *xfer);

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
 *
 * @return 0 on success, or an error code.
 */
int capi_dma_isr(struct capi_dma_handle *handle);

/**
 * @brief DMA Channel Interrupt handler. If interrupt vectors are managed and
 * implemented by user, then user shall call this function in the relevant
 * interrupt vector function.
 *
 * @param [in] chan Points to the DMA channel.
 *
 * @return 0 on success, or an error code.
 */
int capi_dma_isr_chan(struct capi_dma_chan *chan);

/**
 * @brief Registers a callback function for DMA channel interrupts.
 *
 * @param [in] chan Points to the DMA channel.
 * @param [in] callback Callback function to be invoked on DMA interrupts.
 *                     The callback receives the channel and interrupt status.
 * @param [in] xfer_complete_ctx Context data to be passed to the complete callback.
 *
 * @return 0 on success, or an error code.
 */
int capi_dma_register_complete_callback(struct capi_dma_chan *chan,
					capi_dma_xfer_complete_cb callback,
					void *xfer_complete_ctx);

/**
 * @brief Registers a callback function for DMA error events.
 *
 * @param [in] chan Points to the DMA channel.
 * @param [in] callback Callback function to be invoked on DMA errors.
 *                      The callback receives the channel, error status, and context.
 * @param [in] error_ctx Context data to be passed to the error callback.
 *
 * @return 0 on success, or an error code.
 */
int capi_dma_register_error_callback(struct capi_dma_chan *chan,
				     capi_dma_error_cb callback,
				     void *error_ctx);

/**
 * @brief Configures a single scatter-gather descriptor
 *
 * This function builds one hardware descriptor in a scatter-gather chain.
 * Call this function once for each block in the scatter-gather transfer.
 * The descriptors are stored in the array provided in sg_config->extra.
 *
 * @param [in] chan Points to the DMA channel
 * @param [in] sg_config Points to scatter-gather configuration containing the descriptor array
 * @param [in] block Points to the transfer parameters for this descriptor
 * @param [in] index Index of this descriptor in the array (0 to num_blocks-1)
 *
 * @return 0 on success, or an error code.
 */
int capi_dma_config_scattergather_xfer(struct capi_dma_chan *chan,
				       const struct capi_dma_sg_config *sg_config,
				       struct capi_dma_transfer *block, uint32_t index);

/**
 * @brief Starts a scatter-gather DMA transfer
 *
 * This function configures the primary descriptor and starts the scatter-gather
 * DMA operation. All descriptors must be configured using
 * capi_dma_config_scattergather_xfer() before calling this function.
 *
 * @param [in] chan Points to the DMA channel on which the transfer will start
 * @param [in] sg_config Points to scatter-gather configuration containing the descriptor array
 *
 * @return 0 on success, or an error code.
 */
int capi_dma_start_scattergather_xfer(struct capi_dma_chan *chan,
				      const struct capi_dma_sg_config *sg_config);

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
	int (*init_chan)(struct capi_dma_handle *, struct capi_dma_chan **,
			 uint32_t id);
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
	/** Register callback function for channel interrupts */
	int (*register_complete_callback)(struct capi_dma_chan *,
					  capi_dma_xfer_complete_cb callback,
					  void *xfer_complete_ctx);
	/** Register callback function for error events */
	int (*register_error_callback)(struct capi_dma_chan *,
				       capi_dma_error_cb callback,
				       void *error_ctx);
	/** See capi_dma_config_scattergather_xfer() */
	int (*config_scattergather_xfer)(struct capi_dma_chan *,
					 const struct capi_dma_sg_config *,
					 struct capi_dma_transfer *, uint32_t);
	/** See capi_dma_start_scattergather_xfer() */
	int (*start_scattergather_xfer)(struct capi_dma_chan *,
					const struct capi_dma_sg_config *);
};

#if defined(__cplusplus)
}
#endif

#endif
