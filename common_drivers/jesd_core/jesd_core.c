/***************************************************************************//**
* @file jesd_core.c
* @brief Implementation of Jesd Core.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "jesd_core.h"

/* basic read/write functions */

int32_t jesd_read(struct jesd_core *core, uint32_t reg_addr, uint32_t *reg_data) {

	*reg_data = ad_reg_read((core->base_address + reg_addr));
	return(0);
}

int32_t jesd_write(struct jesd_core *core, uint32_t reg_addr, uint32_t reg_data) {

	ad_reg_write((core->base_address + reg_addr), reg_data);
	return(0);
}

/* jesd setup (may be called independent of xcvrs) */

int32_t jesd_setup(struct jesd_core *core) {

	jesd_write(core, JESD204_REG_LINK_DISABLE, JESD204_LINK_DISABLE);
	jesd_write(core, JESD204_REG_LINK_CONFIG_0, (((core->octets_per_frame-1) << 16) |
		((core->frames_per_multiframe*core->octets_per_frame)-1)));
	jesd_write(core, JESD204_REG_LINK_DISABLE, JESD204_LINK_ENABLE);
	return(0);
}

/* jesd status (read after xcvrs are up & running) */
/* it is not much useful to call this function now, the status is not persistent */

int32_t jesd_status(struct jesd_core *core) {

	uint32_t data;
	int32_t timeout;

	timeout = 100;
	while (timeout > 0) {
		jesd_read(core, JESD204_REG_LINK_STATUS, &data);
		if (((data & JESD204_RX_LINK_MASK) == JESD204_RX_LINK_ACTIVE) &&
			(core->tx_or_rx_n == 0)) return(0);
		if (((data & JESD204_TX_LINK_MASK) == JESD204_TX_LINK_ACTIVE) &&
			(core->tx_or_rx_n == 1)) return(0);
		timeout = timeout - 1;
		mdelay(1);
	}
	if (core->tx_or_rx_n == 0) {
		ad_printf("rx_%s: invalid status, data(%x)!\n", __func__, data);
	} else {
		ad_printf("tx_%s: invalid status, data(%x)!\n", __func__, data);
	}
	return(-1);
}

/* jesd sysref control, fpga sourcing sysref */
/* as much as possible do NOT use this method */
/* not guaranteed to have a valid sysref timing if using this method */

int32_t jesd_sysref_control(struct jesd_core *core) {

	ad_gpio_set(core->sysref_gpio_pin, 1);
	mdelay(1);
	ad_gpio_set(core->sysref_gpio_pin, 0);
	return(0);
}

