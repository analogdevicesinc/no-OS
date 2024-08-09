/**
 * @file       sdhc.c
 * @brief      This file contains the function implementations for the
 *             Secure Digital High Capacity (SDHC) peripheral module.
 */

/******************************************************************************
 *
 * Copyright (C) 2022-2023 Maxim Integrated Products, Inc. All Rights Reserved.
 * (now owned by Analog Devices, Inc.),
 * Copyright (C) 2023 Analog Devices, Inc. All Rights Reserved. This software
 * is proprietary to Analog Devices, Inc. and its licensors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

/* **** Includes **** */
#include "sdhc_lib.h"
#include "mxc_assert.h"
#include "mxc_device.h"
#include "mxc_errors.h"
#include "mxc_sys.h"
#include "sdhc.h"
#include "sdhc_resp_regs.h"
#include <stdbool.h>

/**
 * @ingroup sdhc
 * @{
 */

/* **** Definitions **** */

/* SDHC commands and associated cmd register bits which inform hardware to wait
 * for response, etc. */
#define MXC_SDHC_LIB_CMD0 0x0000
#define MXC_SDHC_LIB_CMD1 0x0102
#define MXC_SDHC_LIB_CMD2 0x0209
#define MXC_SDHC_LIB_CMD3 0x031A
#define MXC_SDHC_LIB_CMD4 0x0400
#define MXC_SDHC_LIB_CMD5 0x051A
#define MXC_SDHC_LIB_CMD6 0x060A
#define MXC_SDHC_LIB_CMD7 0x071B
#define MXC_SDHC_LIB_CMD8 0x081A
#define MXC_SDHC_LIB_CMD9 0x0901
#define MXC_SDHC_LIB_CMD10 0x0A01
#define MXC_SDHC_LIB_CMD11 0x0B1A
#define MXC_SDHC_LIB_CMD12 0x0C1B
#define MXC_SDHC_LIB_CMD13 0x0D1A
#define MXC_SDHC_LIB_CMD16 0x101A
#define MXC_SDHC_LIB_CMD17 0x113A
#define MXC_SDHC_LIB_CMD18 0x123A
#define MXC_SDHC_LIB_CMD23 0x171A
#define MXC_SDHC_LIB_CMD24 0x183E
#define MXC_SDHC_LIB_CMD25 0x193E
#define MXC_SDHC_LIB_CMD55 0x371A
/* Application commands (SD Card) which are prefixed by CMD55 */
#define MXC_SDHC_LIB_ACMD6 0x061B
#define MXC_SDHC_LIB_ACMD41 0x2902

#define MXC_SDHC_LIB_BLOCK_SIZE 512

#define MXC_SDHC_LIB_CURRENT_STATE_POS 9
#define MXC_SDHC_LIB_CURRENT_STATE                                            \
  ((uint32_t)(0xFUL << MXC_SDHC_LIB_CURRENT_STATE_POS))

#define MXC_SDHC_LIB_CMD8_ARG1 0x157
#define MXC_SDHC_LIB_DSR 0x04040000

#define MXC_SDHC_LIB_DEFAULT_MAX_BLOCK_SIZE 512
#define MXC_SDHC_LIB_DEFAULT_MAX_BLOCK_REG_VALUE 9
// ACMD41 Command arg
#define MXC_SDHC_LIB_HOST_CAPACITY_SUPPORT_POS 30
#define MXC_SDHC_LIB_HOST_CAPACITY_SUPPORT                                    \
  ((uint32_t)(0x1UL << MXC_SDHC_LIB_HOST_CAPACITY_SUPPORT_POS))
#define MXC_SDHC_LIB_SDXC_POWER_POS 28
#define MXC_SDHC_LIB_SDXC_POWER                                               \
  ((uint32_t)(0x1UL << MXC_SDHC_LIB_SDXC_POWER_POS))

// ACMD41 Response (response reg 1)
#define MXC_SDHC_LIB_ACMD41_BUSY_POS 31
#define MXC_SDHC_LIB_ACMD41_BUSY                                              \
  ((uint32_t)(0x1UL << MXC_SDHC_LIB_ACMD41_BUSY_POS))

/* **** Globals **** */
int card_rca;
mxc_sdhc_lib_card_type card_type = CARD_NONE;
mxc_sdhc_csd_regs_t g_csd;
bool g_csd_is_cached = false;

/* **** Functions **** */

mxc_sdhc_lib_card_type
MXC_SDHC_Lib_Get_Card_Type (void)
{
	return card_type;
}

/* **************************************************************************
 */
int
MXC_SDHC_Lib_SetRCA ()
{
	mxc_sdhc_cmd_cfg_t cmd_cfg;
	int result;

	if (card_type == CARD_MMC) {
		/* We assign an address to MMC cards */
		card_rca = 0x80000000;
	}

	cmd_cfg.direction = MXC_SDHC_DIRECTION_CFG;
	cmd_cfg.arg_1 = card_rca;
	cmd_cfg.host_control_1 = MXC_SDHC_Get_Host_Cn_1 ();
	cmd_cfg.command = MXC_SDHC_LIB_CMD3;
	cmd_cfg.callback = NULL;
	result = MXC_SDHC_SendCommand (&cmd_cfg);

	if ((result == E_NO_ERROR) && (card_type == CARD_SDHC)) {
		/* SD Cards tell us their address */
		card_rca = MXC_SDHC_Get_Response32 ();
	}

	return result;
}

/* **************************************************************************
 */
int
MXC_SDHC_Lib_GetCSD (mxc_sdhc_csd_regs_t *csd)
{
	int result;

	/*
	TODO: We cache the CSD because there seems to be some issues
	    with reading out the CSD after R/W sequences to the card.  We need
	    to investigate this further.
	*/

	if (g_csd_is_cached) {
		*csd = g_csd;
		result = E_NO_ERROR;
	} else {
		mxc_sdhc_cmd_cfg_t cmd_cfg;

		cmd_cfg.direction = MXC_SDHC_DIRECTION_CFG;
		cmd_cfg.host_control_1 = MXC_SDHC_Get_Host_Cn_1 ();
		cmd_cfg.callback = NULL;
		cmd_cfg.arg_1 = card_rca;
		cmd_cfg.command = MXC_SDHC_LIB_CMD9;

		if ((result = MXC_SDHC_SendCommand (&cmd_cfg)) == E_NO_ERROR) {
			MXC_SDHC_Get_Response128 ((unsigned char *)g_csd.array);
			g_csd_is_cached = true;
			*csd = g_csd;
		}
	}

	return result;
}

/* **************************************************************************
 */
unsigned long long
MXC_SDHC_Lib_GetCapacity (mxc_sdhc_csd_regs_t *csd)
{
	unsigned int size = csd->csd.c_size;

	return ((unsigned long long)(size + 1)) * ((unsigned long long)512 * 1024);
}

/* **************************************************************************
 */
unsigned int
MXC_SDHC_Lib_GetSectors (mxc_sdhc_csd_regs_t *csd)
{
	return csd->csd.c_size;
}

/* **************************************************************************
 */
int
MXC_SDHC_Lib_GetBlockSize (mxc_sdhc_csd_regs_t *csd)
{
	unsigned int size = csd->csd.write_bl_len;

	if (size == MXC_SDHC_LIB_DEFAULT_MAX_BLOCK_REG_VALUE) {
		return MXC_SDHC_LIB_DEFAULT_MAX_BLOCK_SIZE;
	} else {
		return E_BAD_PARAM;
	}
}

/* **************************************************************************
 */
int
MXC_SDHC_Lib_GetTaskStatusRegister (uint32_t *tsr)
{
	mxc_sdhc_cmd_cfg_t cmd_cfg;
	int result;

	cmd_cfg.direction = MXC_SDHC_DIRECTION_CFG;
	cmd_cfg.host_control_1 = MXC_SDHC_Get_Host_Cn_1 ();
	cmd_cfg.arg_1 = card_rca;
	cmd_cfg.command = MXC_SDHC_LIB_CMD13;
	cmd_cfg.callback = NULL;
	result = MXC_SDHC_SendCommand (&cmd_cfg);

	*tsr = MXC_SDHC_Get_Response32 ();

	return result;
}

int
MXC_SDHC_Lib_GetCurrentState (mxc_sdhc_state *state)
{
	int result;
	uint32_t tsr;

	result = MXC_SDHC_Lib_GetTaskStatusRegister (&tsr);
	*state = (mxc_sdhc_state)((tsr & MXC_SDHC_LIB_CURRENT_STATE)
				  >> MXC_SDHC_LIB_CURRENT_STATE_POS);

	return result;
}

/* **************************************************************************
 */
int
MXC_SDHC_Lib_SetDsr ()
{
	mxc_sdhc_cmd_cfg_t cmd_cfg;

	cmd_cfg.direction = MXC_SDHC_DIRECTION_CFG;
	cmd_cfg.arg_1 = 0;
	cmd_cfg.host_control_1 = MXC_SDHC_Get_Host_Cn_1 ();
	cmd_cfg.arg_1 = MXC_SDHC_LIB_DSR;
	cmd_cfg.command = MXC_SDHC_LIB_CMD4;
	cmd_cfg.callback = NULL;

	return MXC_SDHC_SendCommand (&cmd_cfg);
}

/* **************************************************************************
 */
int
MXC_SDHC_Lib_SetBusWidth (mxc_sdhc_data_width bus_width)
{
	mxc_sdhc_cmd_cfg_t cmd_cfg;
	uint32_t card_status;
	int result;

	cmd_cfg.direction = MXC_SDHC_DIRECTION_CFG;
	cmd_cfg.callback = NULL;

	if (card_type == CARD_SDHC) {
		cmd_cfg.host_control_1 = MXC_SDHC_Get_Host_Cn_1 ();
		cmd_cfg.arg_1 = card_rca;
		cmd_cfg.command = MXC_SDHC_LIB_CMD55;
		result = MXC_SDHC_SendCommand (&cmd_cfg);

		if (result == E_NO_ERROR) {
			cmd_cfg.host_control_1 |= bus_width;
			cmd_cfg.arg_1 = bus_width;
			cmd_cfg.command = MXC_SDHC_LIB_ACMD6;
			result = MXC_SDHC_SendCommand (&cmd_cfg);
		}
	} else if (card_type == CARD_MMC) {
		cmd_cfg.host_control_1 = MXC_SDHC_Get_Host_Cn_1 ();
		if (bus_width == MXC_SDHC_LIB_SINGLE_DATA) {
			/* 1-bit bus */
			cmd_cfg.arg_1 = 0x03B70000;
		} else {
			/* 4-bit bus */
			cmd_cfg.arg_1 = 0x03B70100;
			cmd_cfg.host_control_1 |= MXC_F_SDHC_HOST_CN_1_DATA_TRANSFER_WIDTH;
		}
		cmd_cfg.command = MXC_SDHC_LIB_CMD6;
		result = MXC_SDHC_SendCommand (&cmd_cfg);
		/* Wait until card busy (D0 low) disappears */
		while (MXC_SDHC_Card_Busy ()) {
		}
		card_status = MXC_SDHC_Get_Response32 ();
		if ((result == E_NO_ERROR) && (card_status & 0x80)) {
			/* SWITCH_ERROR */
			result = E_BAD_STATE;
		}
	} else {
		result = E_BAD_STATE;
	}

	return result;
}

/* **************************************************************************
 */
int
MXC_SDHC_Lib_InitCard (int retries)
{
	mxc_sdhc_cmd_cfg_t cmd_cfg;
	int ocr = 0;
	int R3 = 0;
	int emmc = 0;
	int err;
	int cmd0 = 1;
	uint32_t response;

	g_csd_is_cached = false;
	// Reset CSD cache so that we read CSD at least once on init.

	card_type = CARD_NONE;

	/* SD Cards will reply to CMD8 when in Idle, while (e)MMC will not. Use that
	 * to determine if we're dealing with one or the other.
	 */
	MXC_SDHC_PowerUp ();
	MXC_SDHC_Reset_CMD_DAT ();

	while (retries--) {
		if (cmd0) {
			cmd_cfg.direction = MXC_SDHC_DIRECTION_CFG;
			cmd_cfg.arg_1 = 0;
			cmd_cfg.host_control_1 = MXC_SDHC_Get_Host_Cn_1 ();
			cmd_cfg.command = MXC_SDHC_LIB_CMD0;
			cmd_cfg.callback = NULL;
			if (MXC_SDHC_SendCommand (&cmd_cfg) != E_NO_ERROR) {
				return E_NO_DEVICE;
			}
		}

		if (emmc) {
			/* Try to identify an eMMC card with CMD1 */
			cmd_cfg.arg_1 = 0x00ff8000;
			cmd_cfg.command = MXC_SDHC_LIB_CMD1;
			err = MXC_SDHC_SendCommand (&cmd_cfg);
			if (err == E_NO_ERROR) {
				ocr = MXC_SDHC_Get_Response32 ();
				if ((ocr & 0xff8000) == 0xff8000) {
					/* Card is present, but is it busy? */
					if (ocr & (1UL << 31)) {
						/* Ready! */
						card_type = CARD_MMC;
						break;
					} else {
						cmd0 = 0;
						/* Busy, keep trying until it replies or retries are
						 * exhausted */
					}
				} else {
					/* Next time around, try SD card */
					emmc = 0;
				}
			} else {
				/* Reset hardware for next probe */
				MXC_SDHC_Reset_CMD_DAT ();
				/* Next time around, try SD card */
				emmc = 0;
			}
		} else {
			/* Try to identify a SD card with CMD8 */
			cmd_cfg.arg_1 = MXC_SDHC_LIB_CMD8_ARG1;
			cmd_cfg.command = MXC_SDHC_LIB_CMD8;
			err = MXC_SDHC_SendCommand (&cmd_cfg);
			if (err == E_NO_ERROR) {
				response = MXC_SDHC_Get_Response32 ();
				if (response == MXC_SDHC_LIB_CMD8_ARG1) {
					card_type = CARD_SDHC;
					break;
				}
			} else {
				/* Reset hardware for next probe */
				MXC_SDHC_Reset_CMD_DAT ();
			}
			/* Next time around, try eMMC */
			emmc = 1;
		}
	}

	if (card_type == CARD_NONE) {
		/* Nothing found */
		return E_NO_DEVICE;
	}

	if (card_type == CARD_SDHC) {
		/* SD Card */
		cmd_cfg.arg_1 = 0x0;
		cmd_cfg.command = MXC_SDHC_LIB_CMD55;
		err = MXC_SDHC_SendCommand (&cmd_cfg);
		if (err != E_NO_ERROR) {
			card_type = CARD_NONE;
			return err;
		}

		cmd_cfg.arg_1
			= MXC_SDHC_LIB_SDXC_POWER | MXC_SDHC_LIB_HOST_CAPACITY_SUPPORT;
		cmd_cfg.command = MXC_SDHC_LIB_ACMD41;
		err = MXC_SDHC_SendCommand (&cmd_cfg);
		if (err != E_NO_ERROR) {
			card_type = CARD_NONE;
			return err;
		}

		ocr = MXC_SDHC_Get_Response32 ();
		R3 = 0;
		while (R3 == 0) {
			cmd_cfg.arg_1 = 0x0;
			cmd_cfg.command = MXC_SDHC_LIB_CMD55;
			err = MXC_SDHC_SendCommand (&cmd_cfg);
			if (err != E_NO_ERROR) {
				card_type = CARD_NONE;
				return err;
			}

			cmd_cfg.arg_1 = MXC_SDHC_LIB_SDXC_POWER
					| MXC_SDHC_LIB_HOST_CAPACITY_SUPPORT | ocr;
			cmd_cfg.command = MXC_SDHC_LIB_ACMD41;
			err = MXC_SDHC_SendCommand (&cmd_cfg);
			if (err != E_NO_ERROR) {
				card_type = CARD_NONE;
				return err;
			}
			response = MXC_SDHC_Get_Response32 ();
			R3 = response & MXC_SDHC_LIB_ACMD41_BUSY;
		}
	}

	/* CMD2 to get CID and CMD3 to set card address */
	cmd_cfg.arg_1 = 0x0;
	cmd_cfg.command = MXC_SDHC_LIB_CMD2;
	err = MXC_SDHC_SendCommand (&cmd_cfg);
	if (err != E_NO_ERROR) {
		return err;
	}
	ocr = MXC_SDHC_Get_Response32 ();
	err = MXC_SDHC_Lib_SetRCA ();
	if (err != E_NO_ERROR) {
		return err;
	}

	err = MXC_SDHC_Lib_GetCSD (&g_csd);
	if (err != E_NO_ERROR)
		return err;

	// Calculate clk div to achieve target SDHC clk.
	// First, there is a GCR register setting to determine the input clock to the
	// peripheral. This varies between micros, so a native implementation for
	// each one is maintained in sdhc_mexx.c
	unsigned int sdhc_input_clk_freq = MXC_SDHC_Get_Input_Clock_Freq ();
	MXC_ASSERT (sdhc_input_clk_freq >= SDHC_CLK_FREQ);
	unsigned int sdhc_clk_div = sdhc_input_clk_freq / (2 * SDHC_CLK_FREQ);
	MXC_SDHC_Set_Clock_Config (sdhc_clk_div);

	return E_NO_ERROR;
}

/* **************************************************************************
 */
void
MXC_SDHC_Lib_Async_Handler ()
{
	MXC_SDHC_Handler ();
}

int
MXC_SDHC_Lib_Prepare_Trans (mxc_sdhc_data_width width)
{
	mxc_sdhc_state state;
	mxc_sdhc_cmd_cfg_t cmd_cfg;
	int result;

	cmd_cfg.direction = MXC_SDHC_DIRECTION_CFG;
	cmd_cfg.arg_1 = 0;
	cmd_cfg.host_control_1 = MXC_SDHC_Get_Host_Cn_1 ();
	cmd_cfg.callback = NULL;

	do {
		if ((result = MXC_SDHC_Lib_GetCurrentState (&state)) != E_NO_ERROR) {
			return result;
		}

		/* We expect to be in STBY, TRANS, or PRG state, otherwise failure */
		switch (state) {
		case MXC_SDHC_LIB_STBY_STATE:
			if ((result = MXC_SDHC_Lib_SetDsr ()) != E_NO_ERROR) {
				return result;
			}
			cmd_cfg.arg_1 = card_rca;
			cmd_cfg.command = MXC_SDHC_LIB_CMD7;
			if ((result = MXC_SDHC_SendCommand (&cmd_cfg)) != E_NO_ERROR) {
				return result;
			}
			break;
		case MXC_SDHC_LIB_TRAN_STATE:
			/* No action, ready to go */
			break;
		case MXC_SDHC_LIB_PRG_STATE:
			/* If card is currently in the programming state (writing data to
			 * card) with BUSY=1, block until that is done. This could be
			 * optimized in the case of the Async calls, but that is left as a
			 * future enhancement to the library.
			 */
			if (!MXC_SDHC_Card_Busy ()) {
				/* Bump us out of the loop */
				state = MXC_SDHC_LIB_TRAN_STATE;
			}
			break;
		default:
			/* Other states are errors at this point */
			return E_BAD_STATE;
		}
	} while (state != MXC_SDHC_LIB_TRAN_STATE);

	if ((result = MXC_SDHC_Lib_SetBusWidth (width)) != E_NO_ERROR) {
		return result;
	}

	return E_NO_ERROR;
}

/* **************************************************************************
 */
int
MXC_SDHC_Lib_Write (unsigned int dst_addr, void *src_addr, unsigned int cnt,
		    mxc_sdhc_data_width width)
{
	int result;
	mxc_sdhc_cmd_cfg_t cmd_cfg;

	result = MXC_SDHC_Lib_Prepare_Trans (width);
	if (result != E_NO_ERROR) {
		return result;
	}

	cmd_cfg.host_control_1 = MXC_SDHC_Get_Host_Cn_1 ();
	cmd_cfg.block_size = MXC_SDHC_LIB_BLOCK_SIZE;
	cmd_cfg.sdma = (uint32_t)src_addr;
	cmd_cfg.block_count = cnt;
	cmd_cfg.dma = 1;
	cmd_cfg.direction = MXC_SDHC_DIRECTION_WRITE;
	cmd_cfg.arg_1 = dst_addr;
	if (cnt > 1) {
		cmd_cfg.command = MXC_SDHC_LIB_CMD25;
	} else {
		cmd_cfg.command = MXC_SDHC_LIB_CMD24;
	}
	cmd_cfg.callback = NULL;

	return MXC_SDHC_SendCommand (&cmd_cfg);
}

/* **************************************************************************
 */
int
MXC_SDHC_Lib_Read (void *dst_addr, unsigned int src_addr, unsigned int cnt,
		   mxc_sdhc_data_width width)
{
	int result;
	mxc_sdhc_cmd_cfg_t cmd_cfg;

	result = MXC_SDHC_Lib_Prepare_Trans (width);
	if (result != E_NO_ERROR) {
		return result;
	}

	cmd_cfg.host_control_1 = MXC_SDHC_Get_Host_Cn_1 ();
	cmd_cfg.sdma = (uint32_t)dst_addr;
	cmd_cfg.block_size = MXC_SDHC_LIB_BLOCK_SIZE;
	cmd_cfg.block_count = cnt;
	cmd_cfg.dma = 1;
	cmd_cfg.direction = MXC_SDHC_DIRECTION_READ;
	cmd_cfg.arg_1 = src_addr;
	if (cnt > 1) {
		cmd_cfg.command = MXC_SDHC_LIB_CMD18;
	} else {
		cmd_cfg.command = MXC_SDHC_LIB_CMD17;
	}
	cmd_cfg.callback = NULL;

	return MXC_SDHC_SendCommand (&cmd_cfg);
}

/* **************************************************************************
 */
int
MXC_SDHC_Lib_WriteAsync (unsigned int dst_addr, void *src_addr,
			 unsigned int cnt, mxc_sdhc_data_width width,
			 mxc_sdhc_callback_fn callback)
{
	int data;
	mxc_sdhc_cmd_cfg_t cmd_cfg;

	data = MXC_SDHC_Lib_Prepare_Trans (width);
	if (data == E_BUSY) {
		return E_BUSY;
	}
	if (data == E_BAD_STATE) {
		return E_BAD_STATE;
	}

	cmd_cfg.host_control_1 = MXC_SDHC_Get_Host_Cn_1 ();
	cmd_cfg.block_size = MXC_SDHC_LIB_BLOCK_SIZE;
	cmd_cfg.sdma = (uint32_t)src_addr;
	cmd_cfg.block_count = cnt;
	cmd_cfg.dma = 1;
	cmd_cfg.direction = MXC_SDHC_DIRECTION_WRITE;
	cmd_cfg.arg_1 = dst_addr;
	if (cnt > 1) {
		cmd_cfg.command = MXC_SDHC_LIB_CMD25;
	} else {
		cmd_cfg.command = MXC_SDHC_LIB_CMD24;
	}
	cmd_cfg.callback = callback;

	return MXC_SDHC_SendCommandAsync (&cmd_cfg);
}

/* **************************************************************************
 */
int
MXC_SDHC_Lib_ReadAsync (void *dst_addr, unsigned int src_addr,
			unsigned int cnt, mxc_sdhc_data_width width,
			mxc_sdhc_callback_fn callback)
{
	int data;
	mxc_sdhc_cmd_cfg_t cmd_cfg;

	data = MXC_SDHC_Lib_Prepare_Trans (width);
	if (data == E_BUSY) {
		return E_BUSY;
	}
	if (data == E_BAD_STATE) {
		return E_BAD_STATE;
	}

	cmd_cfg.host_control_1 = MXC_SDHC_Get_Host_Cn_1 ();
	cmd_cfg.sdma = (uint32_t)dst_addr;
	cmd_cfg.block_size = MXC_SDHC_LIB_BLOCK_SIZE;
	cmd_cfg.block_count = cnt;
	cmd_cfg.dma = 1;
	cmd_cfg.direction = MXC_SDHC_DIRECTION_READ;
	cmd_cfg.arg_1 = src_addr;
	if (cnt > 1) {
		cmd_cfg.command = MXC_SDHC_LIB_CMD18;
	} else {
		cmd_cfg.command = MXC_SDHC_LIB_CMD17;
	}
	cmd_cfg.callback = callback;

	return MXC_SDHC_SendCommandAsync (&cmd_cfg);
}
/**@} end of group sdhc */
