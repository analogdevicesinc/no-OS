/***************************************************************************//**
 *   @file   no_os_net.h
 *   @brief  Header file of the network interface
 *   @author alisa.roman@analog.com
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#ifndef _NO_OS_NET_H_
#define _NO_OS_NET_H_

#include <stdint.h>
#include <stdbool.h>

struct no_os_net_desc;
struct no_os_net_init_param;
struct network_interface;

/**
 * @struct no_os_net_ops
 * @brief Structure holding network platform operations function pointers
 */
struct no_os_net_ops {
	/** Initialize the network backend */
	int (*init)(struct no_os_net_desc *, struct no_os_net_init_param *);
	/** Remove backend resources */
	int (*remove)(struct no_os_net_desc *);
	/** Process network stack */
	int (*step)(struct no_os_net_desc *, void *);
};

/**
 * @struct no_os_net_init_param
 * @brief Structure holding the parameters for network initialization
 */
struct no_os_net_init_param {
	/** Network platform operations */
	const struct no_os_net_ops	*platform_ops;
	/** Extra parameters */
	void				*extra;
};

/**
 * @struct no_os_net_desc
 * @brief Structure holding the network descriptor
 */
struct no_os_net_desc {
	/** Network interface */
	struct network_interface	*net_if;
	/** Network platform operations */
	const struct no_os_net_ops	*platform_ops;
	/** Extra parameters */
	void				*extra;
};

/**
 * @brief Initialize the network device
 */
int no_os_net_init(struct no_os_net_desc **desc,
		   struct no_os_net_init_param *param);

/**
 * @brief Remove the network device
 */
int no_os_net_remove(struct no_os_net_desc *desc);

/**
 * @brief Execute network processing step
 */
int no_os_net_step(struct no_os_net_desc *desc, void *arg);

#endif // _NO_OS_NET_H_
