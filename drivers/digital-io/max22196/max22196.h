/***************************************************************************//**
 *   @file   max22196.h
 *   @brief  Header file of MAX22196 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#ifndef _MAX22196_H
#define _MAX22196_H

#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

#define MAX22196_FRAME_SIZE		2

#define MAX22196_CHANNELS		8
#define MAX22194_CHANNELS		4

#define MAX22196_CHN_CNT_RESET		0

#define MAX22196_DI_STATE_REG		0x00
#define MAX22196_FAULT1_REG		0x01
#define MAX22196_F1MASK_REG		0x02
#define MAX22196_CFG_REG(x)		(0x03 + (x))
#define MAX22196_GLOBALCFG_REG		0x0B
#define MAX22196_LED_REG		0x0C
#define MAX22196_FAULT2_REG		0x0D
#define MAX22196_F2MASK_REG		0x0E
#define MAX22196_START_STOP_REG		0x0F
#define MAX22196_CNT_LSB_REG(x)		(0x10 + 2 * (x))
#define MAX22196_CNT_MSB_REG(x)		(0x11 + 2 * (x))

#define MAX22196_ADDR_MASK		NO_OS_GENMASK(7, 6)
#define MAX22196_REG_ADDR_MASK		NO_OS_GENMASK(5, 1)
#define MAX22196_RW_MASK		NO_OS_BIT(0)

#define MAX22196_DI_STATE_MASK(x)	NO_OS_BIT(x)
#define MAX22196_HITHR_MASK		NO_OS_BIT(7)
#define MAX22196_SOURCE_MASK		NO_OS_BIT(6)
#define MAX22196_CURR_MASK		NO_OS_GENMASK(5, 4)
#define MAX22196_FLTEN_MASK		NO_OS_BIT(3)
#define MAX22196_DELAY_MASK		NO_OS_GENMASK(2, 0)
#define MAX22196_CNT_MASK(x)		NO_OS_BIT(x)
#define MAX22196_CFG_MASK		(NO_OS_BIT(7) | NO_OS_GENMASK(5, 4))
#define MAX22196_LSB_MASK		NO_OS_GENMASK(7, 0)
#define MAX22196_MSB_MASK		NO_OS_GENMASK(15, 8)
#define MAX22196_CNT_BYTE_MASK		NO_OS_GENMASK(7, 0)

#define MAX22196_FAULT_MASK(x)		NO_OS_BIT(x)
#define MAX22196_GLOBAL_MASK(x)		NO_OS_BIT(x)
#define MAX22196_FAULT2_MASK		NO_OS_GENMASK(4, 0)

#define MAX22196_FILTER_CLRFLT_MASK	NO_OS_BIT(3)

enum max22196_chip_id {
	ID_MAX22194,
	ID_MAX22196
};

enum max22196_fault_mask {
	MAX22196_GLOBAL_REFDISHTCFG,
	MAX22196_FAULT1_VMLOW,
	MAX22196_FAULT1_V24UV,
	MAX22196_FAULT1_TEMPALM,
	MAX22196_FAULT1_OTSHDN1,
	MAX22196_FAULT1_FAULT2,
	MAX22196_FAULT2_RFDIS,
	MAX22196_FAULT2_RFDIO,
	MAX22196_FAULT2_OTSHDN2,
	MAX22196_FAULT2_SPI8CLK,
	MAX22196_FAULT2_VAUV
};

enum max22196_global_cfg {
	MAX22196_GLOBAL_FSPICLR = 4,
	MAX22196_GLOBAL_LED9,
	MAX22196_GLOBAL_LEDINT,
	MAX22196_GLOBAL_GPO
};

enum max22196_mode {
	MAX22196_SINK_MODE,
	MAX22196_SOURCE_MODE,
};

enum max22196_delay {
	MAX22196_DELAY_50US,
	MAX22196_DELAY_100US,
	MAX22196_DELAY_400US,
	MAX22196_DELAY_800US,
	MAX22196_DELAY_1600US,
	MAX22196_DELAY_3200US,
	MAX22196_DELAY_128000US,
	MAX22196_DELAY_20MS
};

enum max22196_curr {
	MAX22196_HTL_MODE,
	MAX22196_1X_CURRENT,
	MAX22196_3X_CURRENT,
	MAX22196_TTL_OP_OFF
};

struct max22196_init_param {
	uint32_t chip_address;
	struct no_os_spi_init_param *comm_param;
	struct no_os_gpio_init_param *crc_param;
	bool crc_en;
	enum max22196_chip_id chip_id;
};

struct max22196_desc {
	uint32_t chip_address;
	struct no_os_spi_desc *comm_desc;
	struct no_os_gpio_desc *crc_desc;
	uint8_t buff[MAX22196_FRAME_SIZE + 1];
	uint8_t fault2en;
	bool crc_en;
	enum max22196_chip_id chip_id;
	uint8_t max_chn_nb;
};

/** Register write function for MAX22196. */
int max22196_reg_write(struct max22196_desc *, uint32_t, uint32_t);

/** Register read function for MAX22196. */
int max22196_reg_read(struct max22196_desc *, uint32_t, uint32_t *);

/** Register update function for MAX22196. */
int max22196_reg_update(struct max22196_desc *, uint32_t, uint32_t, uint32_t);

/** Set mode to a specific channel. */
int max22196_set_mode(struct max22196_desc *, uint32_t, enum max22196_mode);

/** Set configuration for a specific channel. */
int max22196_chan_cfg(struct max22196_desc *, uint32_t, uint32_t,
		      enum max22196_curr);

/** Set filter values for a specific channel. */
int max22196_filter_set(struct max22196_desc *, uint32_t, uint32_t, uint32_t,
			enum max22196_delay);

/** Read filter values for a specific channel. */
int max22196_filter_get(struct max22196_desc *, uint32_t, uint32_t *,
			uint32_t *, enum max22196_delay *);

/** Set fault masks for requested fault. */
int max22196_fault_mask_set(struct max22196_desc *, enum max22196_fault_mask,
			    bool);

/** Get fault masks state for requested fault. */
int max22196_fault_mask_get(struct max22196_desc *, enum max22196_fault_mask,
			    bool *);

/** Set global configurations manually if desired. */
int max22196_global_cfg(struct max22196_desc *, enum max22196_global_cfg, bool);

/** Set channel counter for a specific channel. */
int max22196_set_chan_cnt(struct max22196_desc *, uint32_t, uint16_t);

/** Read channel counter for a specific channel. */
int max22196_get_chan_cnt(struct max22196_desc *, uint32_t, uint16_t *);

/** Initialize the MAX22196 descriptor. */
int max22196_init(struct max22196_desc **, struct max22196_init_param *);

/** Remove the resources allocated at init. */
int max22196_remove(struct max22196_desc *);

#endif /** _MAX22196_H */
