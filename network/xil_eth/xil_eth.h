/***************************************************************************//**
 * @file xil_eth.h
 * @brief Function to initialize xilinx ethernet.
 * @author Mihail Chindris (mihail.chindris@analog.com)
 ********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * - Neither the name of Analog Devices, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * - The use of this software may or may not infringe the patent rights
 * of one or more patent holders. This license does not release you
 * from the requirement that you obtain separate licenses from these
 * patent holders to use this software.
 * - Use of the software either in source or binary form, must be run
 * on or directly connected to an Analog Devices Inc. component.
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

#ifndef XIL_SOCKET_H_
#define XIL_SOCKET_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "lwip/ip4_addr.h"
#include "network_interface.h"


/* Xilnx ethernet parameters */
#define MAX_SOCKETS		10
#define SOCKET_BUFFER_SIZE	100000

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

struct xil_eth_desc;

struct xil_eth_init_param {
	/* Initialize with MAC address bytes */
	unsigned char mac_ethernet_address[6];
	unsigned emac_baseaddr;
	/* If set to 1, static IP must be set for ip_addr, netmask and gw */
	uint8_t use_static_ip;
	struct ip4_addr ip_addr;
	struct ip4_addr netmask;
	struct ip4_addr gw;
};

int32_t xil_eth_init(struct xil_eth_desc **desc,
		     struct xil_eth_init_param *param);
int32_t xil_eth_remove(struct xil_eth_desc *desc);
/* Proccessing unit. Must be called in order for packets to be procesed. */
int32_t xil_eth_step(struct xil_eth_desc *desc);

int32_t xil_eth_get_ip(struct xil_eth_desc *desc, char *ip_buff,
		       uint32_t buff_size);

int32_t xil_eth_get_network_interface(struct xil_eth_desc *desc,
				      struct network_interface **net);

#endif /* XIL_SOCKET_H_ */
