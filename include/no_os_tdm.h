/***************************************************************************//**
 *   @file   no_os_tdm.h
 *   @brief  Header file of TDM Interface
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _NO_OS_TDM_H_
#define _NO_OS_TDM_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @struct no_os_tdm_platform_ops
 * @brief Structure holding TDM function pointers that point to the platform
 * specific function
 */
struct no_os_tdm_platform_ops;

enum no_os_tdm_mode {
	NO_OS_TDM_MASTER_TX,
	NO_OS_TDM_MASTER_RX,
	NO_OS_TDM_SLAVE_TX,
	NO_OS_TDM_SLAVE_RX
};

/**
 * @struct no_os_tdm_init_param
 * @brief Structure holding the parameters for TDM initialization
 */
struct no_os_tdm_init_param {
	/** TDM operating mode: master/slave, tx/rx */
	enum no_os_tdm_mode mode;
	/** Useful data size in a slot, specified in number of bits */
	uint8_t data_size;
	/** Data offset in a slot, specified in number of bits as offset from MSbit */
	uint8_t data_offset;
	/** Data ordering (default: msb first) */
	bool data_lsb_first;
	/** Number of slots in a frame */
	uint8_t slots_per_frame;
	/** Frame sync polarity specifier (default: FS active high) */
	bool fs_active_low;
	/** Frame sync active length specified in number of bits */
	uint8_t fs_active_length;
	/** Frame sync is normally asserted on first bit of slot 0 of a frame, this specifier allows assertion on last bit of the previous frame */
	bool fs_lastbit;
	/** Specify whether data sampling occurs on SCK rising edge (default: on SCK falling edge) */
	bool rising_edge_sampling;
	/* IRQ ID */
	uint32_t irq_id;
	/** DMA receive complete callback **/
	void (*rx_complete_callback)(void *rx_arg);
	/** DMA receive Half complete callback **/
	void (*rx_half_complete_callback)(void *rx_arg);
	/** Platform operation function pointers */
	const struct no_os_tdm_platform_ops *platform_ops;
	/**  TDM extra parameters (platform specific) */
	void *extra;
};

/**
 * @struct no_os_tdm_desc
 * @brief Structure holding TDM descriptor.
 */
struct no_os_tdm_desc {
	/* IRQ ID */
	uint32_t irq_id;
	/** Platform operation function pointers */
	const struct no_os_tdm_platform_ops *platform_ops;
	/** Software FIFO. */
	struct lf256fifo *rx_fifo;
	/**  TDM extra parameters (device specific) */
	void *extra;
};

/**
 * @struct no_os_tdm_platform_ops
 * @brief Structure holding TDM function pointers that point to the platform
 * specific function.
 */
struct no_os_tdm_platform_ops {
	/** TDM initialization operation function pointer */
	int32_t (*tdm_ops_init)(struct no_os_tdm_desc **,
				const struct no_os_tdm_init_param *);
	/** TDM read operation function pointer */
	int32_t (*tdm_ops_read)(struct no_os_tdm_desc *, void *, uint16_t);
	/** TDM write operation function pointer */
	int32_t (*tdm_ops_write)(struct no_os_tdm_desc *, void *, uint16_t);
	/** Pause TDM DMA transfer */
	int32_t (*tdm_ops_pause)(struct no_os_tdm_desc *);
	/** Resume TDM DMA transfer */
	int32_t (*tdm_ops_resume)(struct no_os_tdm_desc *);
	/** Stop TDM DMA transfer */
	int32_t (*tdm_ops_stop)(struct no_os_tdm_desc *);
	/** TDM remove operation function pointer */
	int32_t (*tdm_ops_remove)(struct no_os_tdm_desc *);
};

/* Initialize the TDM communication peripheral. */
int32_t  no_os_tdm_init(struct no_os_tdm_desc **desc,
			const struct no_os_tdm_init_param *param);

/* Free the resources allocated by  no_os_tdm_init(). */
int32_t  no_os_tdm_remove(struct no_os_tdm_desc *desc);

/* Read data. */
int32_t  no_os_tdm_read(struct no_os_tdm_desc *desc,
			void *data,
			uint16_t bytes_number);

/* Write data. */
int32_t  no_os_tdm_write(struct no_os_tdm_desc *desc,
			 void *data,
			 uint16_t bytes_number);

/* Pause TDM DMA Transfer */
int32_t  no_os_tdm_pause(struct no_os_tdm_desc *desc);

/* Resume TDM DMA Transfer */
int32_t  no_os_tdm_resume(struct no_os_tdm_desc *desc);

/* Stop TDM DMA Transfer */
int32_t  no_os_tdm_stop(struct no_os_tdm_desc *desc);

#endif // _NO_OS_TDM_H_
