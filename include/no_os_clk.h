/***************************************************************************//**
 *   @file   no_os_clk.h
 *   @brief  Header file of Clock Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _NO_OS_CLK_H_
#define _NO_OS_CLK_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/************************* Structure Declarations *****************************/
/******************************************************************************/
struct no_os_clk_init_param {
	/** Device name */
	const char	*name;
	/** Channel number */
	uint8_t		hw_ch_num;
	/** CLK function pointers */
	const struct no_os_clk_platform_ops *platform_ops;
	/**  CLK hardware device descriptor */
	void		*dev_desc;
};

struct no_os_clk_hw {
	void	*dev;
	int32_t (*dev_clk_enable)();
	int32_t (*dev_clk_disable)();
	int32_t (*dev_clk_recalc_rate)();
	int32_t (*dev_clk_set_rate)();
	int32_t (*dev_clk_round_rate)();
};

struct no_os_clk {
	struct no_os_clk_hw	*hw;
	uint32_t	hw_ch_num;
	const char	*name;
	const struct no_os_clk_desc *clk_desc;
};

/**
 * @struct no_os_clk_desc
 * @brief Structure holding CLK descriptor.
 */
typedef struct no_os_clk_desc {
	/** Device name */
	const char	*name;
	/** Channel number */
	uint8_t		hw_ch_num;
	/** CLK function pointers */
	const struct no_os_clk_platform_ops *platform_ops;
	/**  CLK hardware device descriptor */
	void		*dev_desc;
} no_os_clk_desc;

/**
 * @struct no_os_clk_platform_ops
 * @brief Structure holding CLK function pointers that point to the platform
 * specific function
 */
struct no_os_clk_platform_ops {
	/** Initialize CLK function pointer. */
	int (*init)(struct no_os_clk_desc **, const struct no_os_clk_init_param *);
	/** Start CLK function pointer. */
	int (*clk_enable)(struct no_os_clk_desc *);
	/** Stop CLK function pointer. */
	int (*clk_disable)(struct no_os_clk_desc *);
	/** Get the current frequency of CLK function pointer. */
	int (*clk_recalc_rate)(struct no_os_clk_desc *, uint64_t *);
	/* Round the desired frequency to a rate that CLK can actually output. */
	int (*clk_round_rate)(struct no_os_clk_desc *, uint64_t, uint64_t *);
	/* Change CLK frequency function pointer. */
	int (*clk_set_rate)(struct no_os_clk_desc *, uint64_t);
	/** CLK remove function pointer */
	int (*remove)(struct no_os_clk_desc *);
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize CLK ops. */
int32_t no_os_clk_init(struct no_os_clk_desc **desc,
		       const struct no_os_clk_init_param *param);

/* Free the resources allocated by no_os_clk_init(). */
int32_t no_os_clk_remove(struct no_os_clk_desc *desc);

/* Start the clock. */
int32_t no_os_clk_enable(struct no_os_clk_desc *desc);

/* Stop the clock. */
int32_t no_os_clk_disable(struct no_os_clk_desc *desc);

/* Get the current frequency of the clock. */
int32_t no_os_clk_recalc_rate(struct no_os_clk_desc *desc,
			      uint64_t *rate);

/* Round the desired frequency to a rate that the clock can actually output. */
int32_t no_os_clk_round_rate(struct no_os_clk_desc *desc,
			     uint64_t rate,
			     uint64_t *rounded_rate);

/* Change the frequency of the clock. */
int32_t no_os_clk_set_rate(struct no_os_clk_desc *desc,
			   uint64_t rate);

#endif // _NO_OS_CLK_H_
