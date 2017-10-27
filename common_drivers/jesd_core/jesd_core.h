/***************************************************************************//**
 * @file jesd_core.h
 * @brief Header file of Jesd Core.
 * @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
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
#ifndef JESD_CORE_H_
#define JESD_CORE_H_

#include "platform_drivers.h"

#define JESD204_REG_LINK_CONFIG_0			0x210
#define JESD204_REG_LINK_DISABLE			0x0c0
#define JESD204_REG_LINK_STATUS				0x280
#define JESD204_LINK_DISABLE				0x1
#define JESD204_LINK_ENABLE				0x0
#define JESD204_TX_LINK_MASK				0x13
#define JESD204_TX_LINK_ACTIVE				0x13
#define JESD204_RX_LINK_MASK				0x3
#define JESD204_RX_LINK_ACTIVE				0x3

typedef enum {
	EXTERN,
	INTERN
} m_sysref_type;

struct jesd_core {

	uint32_t base_address;
	uint8_t	tx_or_rx_n;
	uint8_t	octets_per_frame;
	uint8_t	frames_per_multiframe;
	uint32_t sysref_gpio_pin;
	m_sysref_type sysref_type;
};

int32_t jesd_read(struct jesd_core *core, uint32_t reg_addr, uint32_t *reg_data);
int32_t jesd_write(struct jesd_core *core, uint32_t reg_addr, uint32_t reg_data);
int32_t jesd_setup(struct jesd_core *core);
int32_t jesd_status(struct jesd_core *core);
int32_t jesd_sysref_control(struct jesd_core *core);

#endif
